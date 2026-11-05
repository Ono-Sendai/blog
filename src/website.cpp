/*=====================================================================
website.cpp
-----------
Copyright Nicholas Chapman 2023 -
=====================================================================*/


#include "WebListenerThread.h"
#include "WebWorkerThreadTests.h"
#include "Escaping.h"
#include "WebsiteExcep.h"
#include "DataStore.h"
#include "BlogRequestHandler.h"
#include <RequestHandler.h>
#include <ThreadManager.h>
#include <PlatformUtils.h>
#include <Networking.h>
#include <Clock.h>
#include <FileUtils.h>
#include <ConPrint.h>
#include <Parser.h>
#include <SocketTests.h>
#include <TestSocket.h>
#include <Base64.h>
#include <TLSSocket.h>
#include <StressTest.h>
#include <OpenSSL.h>
#include <FileWatcherThread.h>

#if TLS_SUPPORT
#include <tls.h>
#endif


#if TLS_SUPPORT
static TLSContextRef createAndConfigureServerTLSContext(const std::string& tls_certificate_path, const std::string& tls_private_key_path)
{
	TLSConfig server_tls_configuration;

	// Set keypair (public cert and private key) paths
	if(tls_config_set_keypair_file(server_tls_configuration.config, tls_certificate_path.c_str(), tls_private_key_path.c_str()) != 0)
		throw web::WebsiteExcep("tls_config_set_keypair_file failed: " + getTLSConfigErrorString(server_tls_configuration.config));

	// Create server TLS context
	TLSContextRef server_tls_context = new TLSContext(tls_server());
	if(!server_tls_context->context)
		throw web::WebsiteExcep("Failed to create tls_context.");

	// Configure it
	if(tls_configure(server_tls_context->context, server_tls_configuration.config) == -1)
		throw web::WebsiteExcep("tls_configure failed: " + getTLSErrorString(server_tls_context));

	// The context holds its own reference to the config now, so it's fine that server_tls_configuration frees it here.

	return server_tls_context;
}
#endif


int main(int argc, char **argv)
{
	Clock::init();
	Networking::init();
	PlatformUtils::ignoreUnixSignals();

	// TEMP: run tests
	if(false)
	{
#if BUILD_TESTS
		//StringUtils::test();
		//SSETest();
		//unsigned int num_threads = PlatformUtils::getNumThreadsInCurrentProcess();
		//printVar(num_threads);
		TestSocket::test();
		WorkerThreadTests::test();
		/*Base64::test();
		Escaping::test();
		Parser::doUnitTests();
		//IPAddress::test();
		Networking::getInstance().doDNSLookup("localhost");
		SocketTests::test();*/
#endif
		return 1; // TEMP
	}

	conPrint("Current working dir: " + PlatformUtils::getCurrentWorkingDirPath());

	try
	{
#if TLS_SUPPORT
		const std::string base_dir_path = PlatformUtils::getResourceDirectoryPath();

		TLSSocket::initTLS(base_dir_path, /*client support=*/false);

#ifdef WIN32
		const std::string tls_certificate_path = "server.cert";
		const std::string tls_private_key_path = "private.key";
#else
		// Use fullchain.pem (our cert followed by the intermediate cert(s)), not cert.pem (just our cert), so that clients can build the chain to a trusted root.
		const std::string tls_certificate_path = "/etc/letsencrypt/live/forwardscattering.org/fullchain.pem";
		const std::string tls_private_key_path = "/etc/letsencrypt/live/forwardscattering.org/privkey.pem";
#endif
		conPrint("Using tls_certificate_path: '" + tls_certificate_path + "'");
		conPrint("Using tls_private_key_path: '" + tls_private_key_path + "'");

		// Create TLS configuration and server context.  This is swapped for a new one when the cert changes (see the file watcher below).
		Reference<CurrentTLSServerContextPointer> cur_tls_context_ptr = new CurrentTLSServerContextPointer();
		cur_tls_context_ptr->setCurrentContext(createAndConfigureServerTLSContext(tls_certificate_path, tls_private_key_path));
#endif

		//tls_config_set_protocols(tls_configuration, TLS_PROTOCOL_TLSv1_2);//TEMP

#ifdef WIN32
		Reference<DataStore> data_store = new DataStore("C:\\programming\\blog\\build\\website.data");
#else
		Reference<DataStore> data_store = new DataStore("website.data");
#endif

#ifdef WIN32		
		data_store->public_files_dir = "C:\\programming\\blog\\build\\blog_public_files";
		data_store->resources_dir    = "C:\\programming\\blog\\build\\resources";
		data_store->letsencrypt_webroot = "/home/nick/letsencrypt_webroot/";
#else
		data_store->public_files_dir = "/home/nick/blog_public_files/";
		data_store->resources_dir    = "/home/nick/blog_resources/";
		data_store->letsencrypt_webroot = "/home/nick/letsencrypt_webroot/";
#endif

		conPrint("Using public_files_dir:    '" + data_store->public_files_dir + "'");
		conPrint("Using resources_dir:       '" + data_store->resources_dir + "'");
		conPrint("Using letsencrypt_webroot: '" + data_store->letsencrypt_webroot + "'");

		Reference<BlogSharedRequestHandler> shared_request_handler = new BlogSharedRequestHandler();
		shared_request_handler->data_store = data_store.getPointer();

		if(FileUtils::fileExists(data_store->path))
			data_store->loadFromDisk();
		else
			conPrint(data_store->path + " not found!");

		ThreadManager thread_manager;
		thread_manager.addThread(new web::WebListenerThread(80,  shared_request_handler.getPointer(), nullptr));
#if TLS_SUPPORT
		thread_manager.addThread(new web::WebListenerThread(443, shared_request_handler.getPointer(), cur_tls_context_ptr));

		// Watch the cert dir, and reload the cert and key when they change, e.g. when certbot renews them.  So the server doesn't need restarting after a renewal.
		// (certbot updates the symlinks in /etc/letsencrypt/live/forwardscattering.org/ to point at the new files in ../../archive/, which gives create/delete events in the live dir.)
		{
			FileWatcherThread::FileWatcherThreadArgs watcher_args;

			FileWatcherThread::WatchGroup group;
			group.dirs.push_back(FileWatcherThread::WatchDir(FileUtils::getDirectory(tls_certificate_path)));
			group.on_change_func = [cur_tls_context_ptr, tls_certificate_path, tls_private_key_path]() {

				// Note that this call will be on the FileWatcherThread.
				// Wait a moment before reloading, so that if both the cert and key are being replaced, both are done.
				// (Further change events queue up during the wait, so this may result in some redundant reloads, which are harmless.)
				PlatformUtils::Sleep(1000);

				conPrint("TLS cert changed, reloading...");
				try
				{
					cur_tls_context_ptr->setCurrentContext(createAndConfigureServerTLSContext(tls_certificate_path, tls_private_key_path));
					conPrint("Reloaded TLS cert.");
				}
				catch(glare::Exception& e)
				{
					conPrint("Exception while creating TLS context with updated cert, keeping current cert: " + e.what());
				}
			};
			watcher_args.watch_groups.push_back(group);

			thread_manager.addThread(new FileWatcherThread(watcher_args));
		}
#endif

		// Stress test
		if(false)
		{
			conPrint("running stress tests...");
			web::StressTest::test(443);
			conPrint("Stress tests done.");
		}
		while(1)
		{
			PlatformUtils::Sleep(10000);
		}
	}
	catch(web::WebsiteExcep& e)
	{
		stdErrPrint("WebsiteExcep: " + e.what());
	}

	Networking::shutdown();
	return 0;
}
