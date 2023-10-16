/*=====================================================================
PageHandlers.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


namespace PageHandlers
{
	void renderShowPagePage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info, unsigned int post_i);
	void handleNewPagePost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderNewPagePage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void handleEditPagePost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderEditPagePage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
} 
