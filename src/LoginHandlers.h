/*=====================================================================
LoginHandlers.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


/*=====================================================================
LoginHandlers
-------------------

=====================================================================*/
namespace LoginHandlers
{
	bool isLoggedInAsNick(DataStore& datastore, const web::RequestInfo& request_info);
	void renderLoginPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void handleLoginPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void handleLogoutPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
}
