/*=====================================================================
BlogHandlers.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "BlogHandlers.h"


#include <ConPrint.h>
#include "RequestInfo.h"
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
#include "Post.h"
#include "DataStore.h"
#include "WebsiteExcep.h"
#include "Escaping.h"
#include "LoginHandlers.h"
#include "ResponseUtils.h"
#include "BlogResponseUtils.h"


namespace BlogHandlers
{


const std::string CRLF = "\r\n";


void renderRootPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	std::string page_out = BlogResponseUtils::standardHeader(data_store, request_info, "Forward Scattering - The Weblog of Nicholas Chapman");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);


	//---------- Right column -------------
	//page_out += "<div class=\"right\">"; // right div


	//-------------- Render posts ------------------
	{
		Lock lock(data_store.mutex);
		for(int post_i = (int)data_store.posts.size() - 1; post_i >= 0; --post_i)
		{
			const Post& post = data_store.posts[post_i];

			if((!post.title.empty() || !post.content.empty()) && (logged_in || post.published))
			{

				page_out += "<div class=\"post\">";
				if(!post.published) page_out += "<div class=\"unpublished\">Unpublished</div>";
				page_out += "<div class=\"post-title\"><a href=\"/post/" + toString(post_i) + "\">" + post.title.HTMLEscaped() + "</a></div>";
				page_out += "<div class=\"timestamp\">Posted " + post.timestamp.dayString() + "</div>";
				page_out += "<div class=\"post-content\">" + post.content.str() + "</div>"; // NOTE: intentionally inserting content without escaping, so can use HTML markup in the post.

				if(logged_in)
				{
					page_out += "<br/>";

					// Edit post link
					page_out += "<small><a href=\"/edit_post?post_id=" + toString(post_i) + "\">Edit post</a></small>&nbsp;|&nbsp;";

					// Delete post link
					page_out += "<small><a href=\"/confirm_delete_post?post_id=" + toString(post_i) + "\">Delete post</a></small>";

				}

				page_out += "<br/>";
				page_out += "</div>";
			}
		}
	
		if(logged_in)
		{
			page_out += "<a href=\"/new_post\">New Post</a> <br/>";
			page_out += "<a href=\"/new_page\">New Page</a> <br/>";
		}

		//page_out += "</div>\n\n"; // end left div
		/*page_out += "<div class=\"left\">"; // Start left div


		// Render page links
		for(int page_i = (int)data_store.pages.size() - 1; page_i >= 0; --page_i)
		{
			const Page& page = data_store.pages[page_i];

			if(!page.url_title.empty() && page.published)
			{
				page_out += "<div class=\"page-link\"><a href=\"/page/" + page.url_title.str() + "\">" + page.title.HTMLEscaped() + "</a></div>";
			}
		}

		page_out += "</div>\n\n"; // End left div*/
	}

	

	
	page_out += BlogResponseUtils::standardFooter(data_store, request_info, true);
	//page_out += "</div>"; // main div
	//page_out += "</body></html>";

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}

	
void renderNotFoundPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	std::string page_out = BlogResponseUtils::standardHeader(data_store, request_info, "Forward Scattering - The Weblog of Nicholas Chapman");

	//---------- Right column -------------
	page_out += "<div class=\"right\">"; // right div


	//-------------- Render posts ------------------
	page_out += "Sorry, the item you were looking for does not exist at that URL.";

	page_out += "</div>"; // end right div
	page_out += BlogResponseUtils::standardFooter(data_store, request_info, true);
	page_out += "</div>"; // main div
	page_out += "</body></html>";

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


}  // end namespace BlogHandlers
