/*=====================================================================
LoginHandlers.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "LoginHandlers.h"


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
#include "ResponseUtils.h"
#include "BlogResponseUtils.h"


namespace LoginHandlers
{


const std::string CRLF = "\r\n";


bool isLoggedInAsNick(DataStore& datastore, const web::RequestInfo& request_info)
{
	for(size_t i=0; i<request_info.cookies.size(); ++i)
	{
		if(request_info.cookies[i].key == "site-a")
		{
			try
			{
				const std::string& session_id = request_info.cookies[i].value;

				Lock lock(datastore.mutex);

				auto res = datastore.user_web_sessions.find(session_id);
				if(res != datastore.user_web_sessions.end())
				{
					UserWebSession* session = res->second.ptr();
					if(session->user_index == 0)
						return true;
				}
			}
			catch(glare::Exception& e)
			{
				conPrint("Error: " + e.what());
			}
		}
	}

	return false;
}


void renderLoginPage(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	std::string page_out = BlogResponseUtils::standardHTMLHeader(data_store, request_info, "Nick's Blog");

	page_out += "<body>";
	page_out += "</head><h1>Log in</h1><body>";
	page_out += "<form action=\"login_post\" method=\"post\">";
	page_out += "email address: <input type=\"text\" name=\"email-address\"><br>";
	page_out += "password: <input type=\"text\" name=\"password\"><br/>";
	page_out += "<input type=\"submit\" value=\"Submit\">";
	page_out += "</form>";
	page_out += "</body></html>";

	web::ResponseUtils::writeHTTPOKHeaderAndData(reply_info, page_out);
}


void handleLoginPost(DataStore& data_store, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	// Try and log in user
	if(request_info.post_fields.size() == 2)
	{
		if(request_info.post_fields[0].key == "email-address" && request_info.post_fields[1].key == "password")
		{
			const web::UnsafeString& email_address = request_info.post_fields[0].value;
			const web::UnsafeString& password =      request_info.post_fields[1].value;
			conPrint("handleLoginPost(): email-address: '" + email_address.str() + "'");

			Lock lock(data_store.mutex);
			if(data_store.users.empty())
				throw glare::Exception("data_store.users empty");
			const User& nick_user = data_store.users[0];

			if(email_address == nick_user.email_address && nick_user.isPasswordValid(password.str()))
			{
				// Valid credentials.
				conPrint("Valid credentials, logging in!");
				web::ResponseUtils::writeRawString(reply_info, "HTTP/1.1 302 Redirect" + CRLF);
				web::ResponseUtils::writeRawString(reply_info, "Location: /" + CRLF);

				UserWebSessionRef session = new UserWebSession();
				session->id = UserWebSession::generateRandomKey();
				session->user_index = 0;
				session->created_time = web::TimeStamp::currentTime();

				data_store.user_web_sessions[session->id] = session;

				web::ResponseUtils::writeRawString(reply_info, "Set-Cookie: site-a=" + session->id + "; Path=/" + CRLF);
				web::ResponseUtils::writeRawString(reply_info, "Content-Length: 0" + CRLF); // NOTE: not sure if content-length is needed for 302 redirect.
				web::ResponseUtils::writeRawString(reply_info, CRLF);
				return;
			}
		}
	}

	// Invalid credentials or some other problem
	conPrint("Invalid credentials.");

	web::ResponseUtils::writeRedirectTo(reply_info, "/");
}
	
	
void handleLogoutPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info)
{
	conPrint("handleLogoutPost");

	web::ResponseUtils::writeRawString(reply_info, "HTTP/1.1 302 Redirect" + CRLF);
	web::ResponseUtils::writeRawString(reply_info, "Location: /" + CRLF);
	web::ResponseUtils::writeRawString(reply_info, "Set-Cookie: site-a=; Path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT" + CRLF); // Clear cookie
	web::ResponseUtils::writeRawString(reply_info, "Content-Length: 0" + CRLF); // NOTE: not sure if content-length is needed for 302 redirect.
	web::ResponseUtils::writeRawString(reply_info, CRLF);
}


} // end namespace LoginHandlers
