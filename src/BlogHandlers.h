/*=====================================================================
BlogHandlers.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


/*=====================================================================
BlogHandlers
-------------------

=====================================================================*/
namespace BlogHandlers
{
	void renderRootPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderNotFoundPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
} 
