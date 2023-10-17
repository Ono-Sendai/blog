/*=====================================================================
RSSHandlers.cpp
---------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "RSSHandlers.h"


#include <ConPrint.h>
#include "RequestInfo.h"
#include <AESEncryption.h>
#include <Exception.h>
#include <MySocket.h>
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


namespace RSSHandlers
{

const std::string CRLF = "\r\n";


// See http://www.rssboard.org/rss-specification
void renderRSSPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	//std::string page_out = BlogResponseUtils::standardHeader(data_store, request_info, socket, "Forward Scattering - The Weblog of Nicholas Chapman");

	std::string response;
	response += "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	response += "<rss version=\"2.0\">\n";
	response += "<channel>\n";
	response += "\t<title>Forward Scattering - The Weblog of Nicholas Chapman</title>\n";
	response += "\t<description>The Weblog of Nicholas Chapman</description>\n";
	response += "\t<link>http://www.forwardscattering.org/</link>\n";
	


	// Get last published post
	std::string last_published_post_date;
	{
		Lock lock(data_store.mutex);

		for(int i=(int)data_store.posts.size()-1; i >= 0; --i)
		{
			const Post& post = data_store.posts[i];
			if((!post.title.empty() || !post.content.empty()) && post.published)
			{
				last_published_post_date = post.timestamp.RFC822FormatedString();
				break;
			}
		}

		response += "\t<lastBuildDate>" + last_published_post_date + "</lastBuildDate>\n"; // The last time the content of the channel changed.

		for(int i=(int)data_store.posts.size()-1; i >= 0; --i)
		{
			const Post& post = data_store.posts[i];
			if((!post.title.empty() || !post.content.empty()) && post.published)
			{
				response += "\t<item>\n";
				response += "\t\t<title>" + post.title.str() + "</title>\n";
				response += "\t\t<link>http://www.forwardscattering.org/post/" + toString(i) + "</link>\n";
				response += "\t\t<description>" + post.content.HTMLEscaped() + "</description>\n"; // NOTE: slow

				response += "\t\t<guid>http://www.forwardscattering.org/post/" + toString(i) + "</guid>\n";
				response += "\t\t<pubDate>" + post.timestamp.RFC822FormatedString() + "</pubDate>\n";
				response += "\t</item>\n";
			}
		}
	}

	response += "</channel>\n";
	response += "</rss>\n";

	//ResponseUtils::writeHTTPOKHeaderAndData(socket, response);

	const std::string header = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: application/rss+xml; charset=utf-8\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Length: " + toString(response.size()) + "\r\n"
		"\r\n";

	reply_info.socket->writeData(header.c_str(), header.size());
	reply_info.socket->writeData(response.c_str(), response.size());
}


} // end PostHandlers
