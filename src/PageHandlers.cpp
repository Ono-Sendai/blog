/*=====================================================================
PageHandlers.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "PageHandlers.h"


#include <ConPrint.h>
#include "RequestInfo.h"
#include <AESEncryption.h>
#include <Exception.h>
#include <mysocket.h>
#include <Lock.h>
#include <Clock.h>
#include <StringUtils.h>
#include <PlatformUtils.h>
#include <KillThreadMessage.h>
#include <Parser.h>
#include <MemMappedFile.h>
#include "RequestInfo.h"
#include "Response.h"
#include "User.h"
#include "DataStore.h"
#include "WebsiteExcep.h"
#include "Escaping.h"
#include "LoginHandlers.h"
#include "ResponseUtils.h"
#include "BlogHandlers.h"
#include "BlogResponseUtils.h"


namespace PageHandlers
{

const std::string CRLF = "\r\n";


void renderShowPagePage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info, unsigned int page_i)
{
	std::string page_out = BlogResponseUtils::standardHeader(data_store, request_info, "Forward Scattering - The Weblog of Nicholas Chapman");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	//-------------- Render page ------------------
	{
		Lock lock(data_store.mutex);

		if(page_i < (unsigned int)data_store.pages.size())
		{
			const Page& page = data_store.pages[page_i];

			if((!page.title.empty() || !page.content.empty()) && (logged_in || page.published))
			{

				page_out += "<div class=\"post\">";
				if(!page.published) page_out += "<div class=\"unpublished\">Unpublished</div>";
				page_out += "<div class=\"post-title\">" + page.title.HTMLEscaped() + "</div>";
				page_out += "<div class=\"timestamp\">Posted " + page.created_timestamp.dayString() + "</div>";
				page_out += "<div class=\"post-content\">" + page.content.str() + "</div>"; // NOTE: intentionally inserting content without escaping, so can use HTML markup in the page.

				if(logged_in)
				{
					page_out += "<br/>";

					// Edit page link
					page_out += "<small><a href=\"/edit_page?page_id=" + toString(page_i) + "\">Edit page</a></small>";

					// Delete post link
					//page_out += "<small><a href=\"/confirm_delete_post?post_id=" + toString(post_i) + "\">Delete post</a></small>";
				}
		
				page_out += "<br/>"
					"</div>"; // End post div
			}
		}
		else
		{
			// Page id is not valid
			page_out += "<p>No such page exists.</p>";
		}
	}

	page_out += "<p>"
		"<i>Do you have a comment or feedback about this page?  Please <a href=\"mailto:nick@indigorenderer.com\">email me</a>.</i>"
		"<p>"
		"<p>"
		"<a href=\"/\">&lt; Back</a>"
		"<p>";

	page_out += BlogResponseUtils::standardFooter(data_store, request_info, false);

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


void handleNewPagePost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	conPrint("handleNewPagePost");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	if(logged_in)
	{
		Page page;
		page.url_title = request_info.getPostField("url_title");
		page.title = request_info.getPostField("title");
		page.content = request_info.getPostField("content");
		page.created_timestamp = web::TimeStamp::currentTime();
		page.last_modified_timestamp = page.created_timestamp;
		page.published = request_info.getPostField("published") == "checked";

		Lock lock(data_store.mutex);
		data_store.pages.push_back(page);

		// Write data store to disk
		try
		{
			data_store.writeToDisk();
		}
		catch(web::WebsiteExcep& e)
		{
			conPrint("Failed to write datastore to disk: " + e.what());
		}

		conPrint("Added new page!");
		web::ResponseUtils::writeRedirectTo(reply_info, "/page/" + toString(data_store.pages.size() - 1));
	}
	else
	{
		web::ResponseUtils::writeRedirectTo(reply_info, "/");
	}
}


void renderNewPagePage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	std::string page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "New Page");

	page_out += "<body>";
	page_out += "</head><h1>New Page</h1><body>";

	page_out += "<form action=\"new_page_post\" method=\"post\">\n";
	
	page_out += "URL Title: <input size=\"80\" type=\"text\" name=\"url_title\" />   <br/>\n";
	page_out += "Title: <input size=\"80\" type=\"text\" name=\"title\" />   <br/>\n";
	page_out += "Content: <textarea rows=\"20\" cols=\"80\" name=\"content\"></textarea>   <br/>\n";
	page_out += "Published: <input type=\"checkbox\" name=\"published\" value=\"checked\" ><br/>\n";
	page_out += "<input type=\"submit\" value=\"Submit\" / >\n";
	
	page_out += "</form>";

	page_out += "</body></html>";

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


void handleEditPagePost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	conPrint("handleEditPagePost");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	try
	{
		if(logged_in)
		{
			const int page_i = request_info.getURLIntParam("page_id");

			{
				Lock lock(data_store.mutex);

				if(page_i >= 0 && page_i < (int)data_store.pages.size())
				{
					Page& page = data_store.pages[page_i];

					page.url_title = request_info.getPostField("url_title");
					page.title = request_info.getPostField("title");
					page.content = request_info.getPostField("content");
					page.published = request_info.getPostField("published") == "checked";
					page.last_modified_timestamp = web::TimeStamp::currentTime();

					// Write data store to disk
					data_store.writeToDisk();
					data_store.updateAssetManagerAndPageURLMap();

					conPrint("Edited page.");
				
				}
			}

			web::ResponseUtils::writeRedirectTo(reply_info, "/page/" + toString(page_i));
			return;
		}
		else
		{

		}
	}
	catch(StringUtilsExcep& )
	{}

	web::ResponseUtils::writeRedirectTo(reply_info, "/");
}


void renderEditPagePage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	try
	{
		std::string page_out;
		const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);
		if(logged_in)
		{
			const int page_i = request_info.getURLIntParam("page_id");

			Lock lock(data_store.mutex);

			if(page_i >= 0 && page_i < (int)data_store.pages.size())
			{
				const Page& page = data_store.pages[page_i];

				page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "Edit Page");
				
				page_out += "<body>";
				page_out += "</head><h1>Edit Page</h1><body>";

				page_out += "<form action=\"edit_page_post?page_id=" + toString(page_i) + "\" method=\"post\">\n";
	
				page_out += "URL Title: <textarea rows=\"1\" cols=\"80\" name=\"url_title\">" + page.url_title.HTMLEscaped() + "</textarea><br/>\n";
				page_out += "Title: <textarea rows=\"1\" cols=\"80\" name=\"title\">" + page.title.HTMLEscaped() + "</textarea><br/>\n";
				page_out += "Content: <textarea rows=\"20\" cols=\"80\" name=\"content\">" + page.content.HTMLEscaped() + "</textarea>   <br/>\n";
				page_out += std::string("Published: <input type=\"checkbox\" name=\"published\" value=\"checked\" ") + (page.published ? "checked" : "") + "><br/>\n";
				page_out += "<input type=\"submit\" value=\"Submit\" / >\n";
	
				page_out += "</form>";

				page_out += "</body></html>";
			}
		}

		web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
	}
	catch(StringUtilsExcep& )
	{}	
}


} // end PageHandlers


