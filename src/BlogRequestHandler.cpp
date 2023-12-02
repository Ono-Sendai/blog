/*=====================================================================
BlogRequestHandler.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "BlogRequestHandler.h"


#include "User.h"
#include "Post.h"
#include "DataStore.h"
#include "WebsiteExcep.h"
#include "Escaping.h"
#include "PostHandlers.h"
#include "RSSHandlers.h"
#include "LoginHandlers.h"
#include "ResponseUtils.h"
#include "RequestHandler.h"
#include "BlogHandlers.h"
#include "PageHandlers.h"
#include <Escaping.h>
#include <StringUtils.h>
#include <Parser.h>
#include <MemMappedFile.h>
#include <ConPrint.h>
#include <FileUtils.h>
#include <Exception.h>
#include <Lock.h>


BlogRequestHandler::BlogRequestHandler()
{
}


BlogRequestHandler::~BlogRequestHandler()
{
}


static bool isLetsEncryptFileQuerySafe(const std::string& s)
{
	for(size_t i=0; i<s.size(); ++i)
		if(!(::isAlphaNumeric(s[i]) || (s[i] == '-') || (s[i] == '_') || (s[i] == '.')))
			return false;
	return true;
}


static bool isFileNameSafe(const std::string& s)
{
	for(size_t i=0; i<s.size(); ++i)
		if(!(::isAlphaNumeric(s[i]) || (s[i] == '-') || (s[i] == '_') || (s[i] == '.')))
			return false;
	return true;
}


void BlogRequestHandler::handleRequest(const web::RequestInfo& request, web::ReplyInfo& reply_info)
{
	if(request.verb == "POST")
	{
		// Route PUT request
		if(request.path == "/login_post")
		{
			LoginHandlers::handleLoginPost(*data_store, request, reply_info);
		}
		else if(request.path == "/logout_post")
		{
			LoginHandlers::handleLogoutPost(*data_store, request, reply_info);
		}
		else if(request.path == "/new_post_post")
		{
			PostHandlers::handleNewPostPost(*data_store, request, reply_info);
		}
		else if(request.path == "/edit_post_post")
		{
			PostHandlers::handleEditPostPost(*data_store, request, reply_info);
		}
		else if(request.path == "/delete_post_post")
		{
			PostHandlers::deletePostPost(*data_store, request, reply_info);
		}
		else if(request.path == "/new_page_post")
		{
			PageHandlers::handleNewPagePost(*data_store, request, reply_info);
		}
		else if(request.path == "/edit_page_post")
		{
			PageHandlers::handleEditPagePost(*data_store, request, reply_info);
		}
		else
		{
			std::string page = "Unknown post URL";
			web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page);
		}
	}
	else if(request.verb == "GET")
	{
		// conPrint("path: " + request.path); //TEMP

		// Look up static assets
		{
			//TEMP: NO LOCK as static_asset_manager won't be getting updated Lock lock(data_store->mutex);
			const std::map<std::string, web::StaticAssetRef>::const_iterator res = data_store->static_asset_manager.getStaticAssets().find(request.path);
			if(res != data_store->static_asset_manager.getStaticAssets().end())
			{
				const web::StaticAsset& asset = *res->second;

				// NOTE: this entire string could be precomputed.
				web::ResponseUtils::writeRawString(reply_info,
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: " + asset.mime_type + "\r\n" +
					"Connection: Keep-Alive\r\n"
					"Content-Length: " + toString(asset.file->fileSize()) + "\r\n"
					"Cache-Control: max-age=3600" + "\r\n"
					//"Expires: Thu, 15 Apr 2020 20:00:00 GMT\r\n"
					//"ETag: \"" + asset.etag + "\"\r\n"
					"\r\n"
				);

				// Write out the actual file data.
				reply_info.socket->writeData(asset.file->fileData(), asset.file->fileSize());
			
				return;
			}
		}

		// Route GET request
		if(request.path == "/")
		{
			BlogHandlers::renderRootPage(*data_store, request, reply_info);
		}
		else if(request.path == "/login")
		{
			LoginHandlers::renderLoginPage(*data_store, request, reply_info);
		}
		// Handle Post-related GETs
		else if(request.path == "/new_post")
		{
			PostHandlers::renderNewPostPage(*data_store, request, reply_info);
		}
		else if(request.path == "/edit_post")
		{
			PostHandlers::renderEditPostPage(*data_store, request, reply_info);
		}
		else if(request.path == "/confirm_delete_post")
		{
			PostHandlers::renderConfirmDeletePost(*data_store, request, reply_info);
		}
		else if(::hasPrefix(request.path, "/post/"))
		{
			// Special show post URL
			const size_t offset = 6;
			assert(std::string("/post/").size() == offset);
			Parser p(request.path.c_str() + offset, request.path.size() - offset); // Start parsing after "/post/"

			// Parse post id
			unsigned int post_id;
			if(!p.parseUnsignedInt(post_id))
			{
				BlogHandlers::renderNotFoundPage(*data_store, request, reply_info);
			}
			else
			{
				PostHandlers::renderShowPostPage(*data_store, request, reply_info, post_id);
			}
		}
		// Handle page-related GETs
		else if(request.path == "/new_page")
		{
			PageHandlers::renderNewPagePage(*data_store, request, reply_info);
		}
		else if(request.path == "/edit_page")
		{
			PageHandlers::renderEditPagePage(*data_store, request, reply_info);
		}
		else if(::hasPrefix(request.path, "/page/"))
		{
			// Special show page URL
			const size_t offset = 6;
			assert(std::string("/page/").size() == offset);
			Parser p(request.path.c_str() + offset, request.path.size() - offset); // Start parsing after "/page/"

			if(!p.eof() && isNumeric(p.current()))
			{
				// Parse page id
				unsigned int page_id;
				if(!p.parseUnsignedInt(page_id))
					throw web::WebsiteExcep("Invalid page id");

				PageHandlers::renderShowPagePage(*data_store, request, reply_info, page_id);
			}
			else
			{
				string_view url_title;
				p.parseToCharOrEOF('/', url_title);

				const std::string unescaped_URL_title = web::Escaping::URLUnescape(url_title.to_string());

				// Look up in page_url_map
				int page_id = -1;
				{
					Lock lock(data_store->mutex);
					auto res = data_store->page_url_map.find(unescaped_URL_title);
					if(res != data_store->page_url_map.end())
						page_id = res->second;
				}

				if(page_id == -1)
					BlogHandlers::renderNotFoundPage(*data_store, request, reply_info);
				else
					PageHandlers::renderShowPagePage(*data_store, request, reply_info, page_id);
			}
		}
		// RSS
		else if(::hasPrefix(request.path, "/rss"))
		{
			RSSHandlers::renderRSSPage(*data_store, request, reply_info);
		}
		else if(::hasPrefix(request.path, "/files/"))
		{
			const std::string filename = ::eatPrefix(request.path, "/files/");
			if(isFileNameSafe(filename))
			{
				try
				{
					MemMappedFile file(data_store->public_files_dir + "/" + filename);
					std::string content_type;
					if(::hasExtension(filename, "png"))
						content_type = "image/png";
					else if(::hasExtension(filename, "jpg"))
						content_type = "image/jpeg";
					else if(::hasExtension(filename, "pdf"))
						content_type = "application/pdf";
					else
						content_type = "bleh";

					web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, file.fileData(), file.fileSize(), content_type.c_str());
				}
				catch(glare::Exception&)
				{
					web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, "Failed to load file '" + filename + "'.");
				}
			}
			else
			{
				web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, "invalid/unsafe filename");
				return;
			}
		}
		else if(::hasPrefix(request.path, "/.well-known/acme-challenge/")) // Support for Let's encrypt: Serve up challenge response file.
		{
			const std::string filename = ::eatPrefix(request.path, "/.well-known/acme-challenge/");
			if(!isLetsEncryptFileQuerySafe(filename))
			{
				web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, "invalid/unsafe file query");
				return;
			}

			// Serve up the file
			try
			{
				std::string contents;
				FileUtils::readEntireFile(data_store->letsencrypt_webroot + "/.well-known/acme-challenge/" + filename, contents);
				web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, contents);
			}
			catch(FileUtils::FileUtilsExcep& e)
			{
				web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, "Failed to load file '" + filename + "': " + e.what());
			}
		}
		else
		{
			std::string page = "Unknown page";
			web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page);
		}
	}
}


void BlogRequestHandler::handleWebsocketTextMessage(const std::string& msg, Reference<SocketInterface>& socket, const Reference<WorkerThread>& worker_thread)
{}


void BlogRequestHandler::websocketConnectionClosed(Reference<SocketInterface>& socket, const Reference<WorkerThread>& worker_thread)
{}
