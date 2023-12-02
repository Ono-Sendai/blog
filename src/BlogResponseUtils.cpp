/*=====================================================================
BlogResponseUtils.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "BlogResponseUtils.h"


#include <ConPrint.h>
#include <Clock.h>
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
#include "Escaping.h"
#include "LoginHandlers.h"
#include "DataStore.h"
#include "Page.h"


namespace BlogResponseUtils
{


const std::string CRLF = "\r\n";


const std::string standardHTMLHeader(DataStore& datastore, const web::RequestInfo& request_info, const std::string& page_title)
{
	// See https://www.mathjax.org/cdn-shutting-down/

	return
"<!DOCTYPE html>\n\
<html><head>\n\
<link href=\"/site.css\" rel=\"stylesheet\" type=\"text/css\">\n\
<script type=\"text/javascript\" src=\"https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>\n\
<title>" + page_title + "</title>\n\
</head>\n";
}


const std::string standardHeader(DataStore& datastore, const web::RequestInfo& request_info, const std::string& page_title)
{
	std::string page_out = BlogResponseUtils::standardHTMLHeader(datastore, request_info, "Forward Scattering - The Weblog of Nicholas Chapman");
	page_out += "<body>\n"

		"<div class=\"main\">\n" // Start main div
		"<table><tr><td style=\"vertical-align: top;\">" // Start table and left cell
	
		"<div class=\"left\">" // Start left column

		"<div class=\"header\"></div>\n" // Header div

		"<div id=\"blogtitle\">\n"
		"    <a href=\"/\"><img src=\"/title-image_600.png\" alt=\"title image\" /></a>\n"
		"</div>\n"

		"<div id=\"subtitle\"><i>The weblog of Nicholas Chapman</i></div>"

		// Render if user is logged in.
		"<div id=\"login\">\n";

	const bool logged_in = LoginHandlers::isLoggedInAsNick(datastore, request_info);

	if(logged_in)
	{
		page_out += "You are logged in as nick\n";

		// Add logout button
		page_out += "<form action=\"logout_post\" method=\"post\">\n";
		page_out += "<input class=\"link-button\" type=\"submit\" value=\"Log out\">\n";
		page_out += "</form>\n";
	}
	else
	{
		//page_out += "<a href=\"/login\">log in</a> <br/>\n";
	}
	page_out += "</div>\n" // End login div

		"<div id=\"header-image\"></div>\n";

	return page_out;
}


const std::string standardFooter(DataStore& datastore, const web::RequestInfo& request_info, bool include_email_link)
{
	std::string page_out;
	if(include_email_link)
	{
		page_out += 
			"<div class=\"footer\"> \n\
			All content by Nicholas Chapman. &nbsp; <a href=\"mailto:nick@indigorenderer.com\">Email me</a>. &nbsp; <a href=\"/rss\">RSS feed</a>  \n\
			</div>\n";
	}
	else
	{
		page_out +=
			"<div class=\"footer\"> \n\
			All content by Nicholas Chapman.\n\
			</div>\n";
	}

	page_out += "</div>\n\n" // End left column div
		"</td><td style=\"vertical-align: top; padding-top: 100px;\">\n" // End left table cell, start right table cell

		"<div class=\"right\">\n"

		"<div class=\"right-link-group\">\n"
		"<div class=\"right-subtitle\">Pages</div>\n";

	{
		Lock lock(datastore.mutex);
		for(int page_i = (int)datastore.pages.size() - 1; page_i >= 0; --page_i)
		{
			const Page& page = datastore.pages[page_i];
			if(!page.url_title.empty() && page.published)
			{
				page_out += "<div class=\"page-link\"><a href=\"/page/" + page.url_title.str() + "\">" + page.title.HTMLEscaped() + "</a></div>";
			}
		}
	}

	page_out += 
		"</div>\n" // End right-link-group from page list above
		"<div class=\"right-link-group\">\n"
		"<div class=\"right-subtitle\">Social media</div>\n"
		"<div class=\"external-link\"><a href=\"https://twitter.com/NickChapmn\">Twitter / X</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://github.com/Ono-Sendai\">GitHub</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://www.youtube.com/channel/UCMbMb_6qsPxE957V8u6MJ0Q\">YouTube</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://www.flickr.com/photos/onosendai/\">Flickr</a></div>\n"
		"</div>\n" // End right-link-group

		"<div class=\"right-link-group\">\n"
		"<div class=\"right-subtitle\">Our software</div>\n"
		"<div class=\"external-link\"><a href=\"http://substrata.info\">Substrata</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://indigorenderer.com\">Indigo Renderer</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://chaoticafractals.com\">Chaotica</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://github.com/glaretechnologies/winter\">Winter</a></div>\n"
		"</div>\n" // End right-link-group

		"<div class=\"right-link-group\">\n"
		"<div class=\"right-subtitle\">Other blogs</div>\n"
		"<div class=\"external-link\"><a href=\"https://www.cbloom.com/rants.html\">CBloom rants</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://conorstokes.github.io/\">Conor Stokes blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://randomascii.wordpress.com/\">Random ASCII</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://blog.regehr.org/\">Embedded in Academia</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://preshing.com/\">Preshing on Programming</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://algorithmicassertions.com/\">Algorithmic Assertions</a></div>\n"
		"<div class=\"external-link\"><a href=\"http://reedbeta.com/\">Nathan Reed's blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://fgiesen.wordpress.com/\">The ryg blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://zeux.io/\">Bits, pixels, cycles and more (by Arseny K.)</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://bartwronski.com/\">Bart Wronski's blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://www.rfleury.com/\">Hidden Grove</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://aras-p.info/blog/\">Aras P's blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://neugierig.org/software/blog/\">Neugierig.org: Tech Notes</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://nigeltao.github.io/\">nigeltao.github.io</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://maxliani.wordpress.com/\">Art, Tech and other Nonsense (by Max Liani)</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://medium.com/@jasonbooth_86226\">Jason Booth's blog</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://therealmjp.github.io/posts/\">The Danger Zone</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://www.jendrikillner.com/#posts\">Graphics Programming weekly</a></div>\n"
		"<div class=\"external-link\"><a href=\"https://blog.demofox.org/\">The blog at the bottom of the sea</a></div>\n"
		"</div>\n" // End right-link-group

		"</div>\n\n" // End right div
		"</td></tr></table>\n\n" // End right table cell and table

		"</div>\n" // End main div
		"</body></html>";

	return page_out;
	/*return
		"<div class=\"footer\"> \n\
		<a href=\"mailto: nick@indigorenderer.com\">Email me</a>. \n\
		</div>\n";*/
}


} // end namespace BlogResponseUtils
