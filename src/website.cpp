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
#include <networking.h>
#include <Clock.h>
#include <FileUtils.h>
#include <ConPrint.h>
#include <Parser.h>
#include <networking.h>
#include <SocketTests.h>
#include <TestSocket.h>
#include <Base64.h>
#include <TLSSocket.h>
#include <StressTest.h>
#include <OpenSSL.h>

#if TLS_SUPPORT
#include <tls.h>
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
		TLSSocket::initTLS();

		// Create TLS configuration
		struct tls_config* tls_configuration = tls_config_new();

#ifdef WIN32
		if(tls_config_set_cert_file(tls_configuration, "server.cert") != 0)
			throw web::WebsiteExcep("tls_config_set_cert_file failed.");

		if(tls_config_set_key_file(tls_configuration, "private.key") != 0)
			throw web::WebsiteExcep("tls_config_set_key_file failed.");
#else
		if(tls_config_set_cert_file(tls_configuration, "/etc/letsencrypt/live/forwardscattering.org/cert.pem") != 0)
			throw web::WebsiteExcep("tls_config_set_cert_file failed.");

		if(tls_config_set_key_file(tls_configuration, "/etc/letsencrypt/live/forwardscattering.org/privkey.pem") != 0)
			throw web::WebsiteExcep("tls_config_set_key_file failed.");
#endif

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
		thread_manager.addThread(new web::WebListenerThread(80,  shared_request_handler.getPointer(), NULL));
#if TLS_SUPPORT
		thread_manager.addThread(new web::WebListenerThread(443, shared_request_handler.getPointer(), tls_configuration));
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
