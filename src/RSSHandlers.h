/*=====================================================================
RSSHandlers.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


namespace RSSHandlers
{
	void renderRSSPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
} 
