/*=====================================================================
PostHandlers.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "PostHandlers.h"


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
#include "Post.h"
#include "DataStore.h"
#include "WebsiteExcep.h"
#include "Escaping.h"
#include "LoginHandlers.h"
#include "ResponseUtils.h"
#include "BlogHandlers.h"
#include "BlogResponseUtils.h"


namespace PostHandlers
{

const std::string CRLF = "\r\n";


void renderShowPostPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info, unsigned int post_i)
{
	std::string page_out = BlogResponseUtils::standardHeader(data_store, request_info, "Forward Scattering - The Weblog of Nicholas Chapman");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	//-------------- Render post ------------------
	{
		Lock lock(data_store.mutex);

		if(post_i < (unsigned int)data_store.posts.size())
		{
			const Post& post = data_store.posts[post_i];

			if((!post.title.empty() || !post.content.empty()) && (logged_in || post.published))
			{

				page_out += "<div class=\"post\">";
				if(!post.published) page_out += "<div class=\"unpublished\">Unpublished</div>";
				page_out += "<div class=\"post-title\">" + post.title.HTMLEscaped() + "</div>";
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
		
				page_out += "<br/>"
					"</div>"; // End post div
			}
		}
		else
		{
			// Post id is not valid
			page_out += "<p>No such post exists.</p>";
		}
	}

	page_out += "<p>"
		"<i>Do you have a comment or feedback about this blog post?  Please <a href=\"mailto:nick@indigorenderer.com\">email me</a>.</i>"
		"<p>"
		"<p>"
		"<a href=\"/\">&lt; Back</a>"
		"<p>";

	page_out += BlogResponseUtils::standardFooter(data_store, request_info, false);

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


void handleNewPostPost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	conPrint("handleNewPostPost");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	if(logged_in)
	{
		Post post;
		post.title = request_info.getPostField("title");
		post.content = request_info.getPostField("content");
		post.timestamp = web::TimeStamp::currentTime();

		Lock lock(data_store.mutex);
		data_store.posts.push_back(post);

		// Write data store to disk
		try
		{
			data_store.writeToDisk();
		}
		catch(web::WebsiteExcep& e)
		{
			conPrint("Failed to write datastore to disk: " + e.what());
		}

		conPrint("Added new post!");
	}
	else
	{

	}

	web::ResponseUtils::writeRedirectTo(reply_info, "/");
}


void renderNewPostPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	std::string page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "New Post");

	page_out += "<body>";
	page_out += "</head><h1>New Post</h1><body>";

	page_out += "<form action=\"new_post_post\" method=\"post\">\n";
	
	page_out += "Title: <input size=\"80\" type=\"text\" name=\"title\" />   <br/>\n";
	page_out += "Content: <textarea rows=\"20\" cols=\"80\" name=\"content\"></textarea>   <br/>\n";
	page_out += "Published: <input type=\"checkbox\" name=\"published\" value=\"checked\" ><br/>\n";
	page_out += "<input type=\"submit\" value=\"Submit\" / >\n";
	
	page_out += "</form>";

	page_out += "</body></html>";

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


void handleEditPostPost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	conPrint("handleEditPostPost");
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	try
	{
		if(logged_in)
		{
			const int post_i = request_info.getURLIntParam("post_id");

			{
				Lock lock(data_store.mutex);

				if(post_i >= 0 && post_i < (int)data_store.posts.size())
				{
					Post& post = data_store.posts[post_i];

					post.title = request_info.getPostField("title");
					post.content = request_info.getPostField("content");
					post.published = request_info.getPostField("published") == "checked";

					// Write data store to disk
					data_store.writeToDisk();

					conPrint("Edited post.");
				
				}
			}

			web::ResponseUtils::writeRedirectTo(reply_info, "/post/" + toString(post_i));
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


void renderConfirmDeletePost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	try
	{
		if(logged_in)
		{
			const int post_i = request_info.getURLIntParam("post_id");

			std::string page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "Confirm Delete Post Request");
			
			page_out += "<body>";
			page_out += "</head><h1>Delete Post</h1><body>";

			page_out += "<form action=\"delete_post_post?post_id=" + toString(post_i) + "\" method=\"post\">\n";
	
			page_out += "Are you sure you want to delete the post?<br/>\n";
			page_out += "<input type=\"submit\" value=\"Delete Post\" / >\n";
			
			page_out += "</form>";

			page_out += "<a href=\"/\">Cancel</a>";

			page_out += "</body></html>";

			web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
		}
		else
		{

		}
	}
	catch(StringUtilsExcep& )
	{}
}


void deletePostPost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);

	try
	{
		if(logged_in)
		{
			const int post_i = request_info.getURLIntParam("post_id");

			Lock lock(data_store.mutex);

			if(post_i >= 0 && post_i < (int)data_store.posts.size())
			{
				Post& post = data_store.posts[post_i];

				// 'Delete' post by setting all fields to empty string
				post.title = "";
				post.content = "";

				// Write data store to disk
				data_store.writeToDisk();

				conPrint("Deleted post.");
			}
		}
		else
		{

		}
	}
	catch(StringUtilsExcep& )
	{}

	web::ResponseUtils::writeRedirectTo(reply_info, "/");
}


void renderEditPostPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	try
	{
		std::string page_out;
		const bool logged_in = LoginHandlers::isLoggedInAsNick(data_store, request_info);
		if(logged_in)
		{
			const int post_i = request_info.getURLIntParam("post_id");

			Lock lock(data_store.mutex);

			if(post_i >= 0 && post_i < (int)data_store.posts.size())
			{
				const Post& post = data_store.posts[post_i];

				page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "Edit Post");
				
				page_out += "<body>";
				page_out += "</head><h1>Edit Post</h1><body>";

				page_out += "<form action=\"edit_post_post?post_id=" + toString(post_i) + "\" method=\"post\">\n";
	
				page_out += "Title: <textarea rows=\"1\" cols=\"80\" name=\"title\">" + post.title.HTMLEscaped() + "</textarea><br/>\n";
				page_out += "Content: <textarea rows=\"20\" cols=\"80\" name=\"content\">" + post.content.HTMLEscaped() + "</textarea>   <br/>\n";
				page_out += std::string("Published: <input type=\"checkbox\" name=\"published\" value=\"checked\" ") + (post.published ? "checked" : "") + "><br/>\n";
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


} // end PostHandlers


