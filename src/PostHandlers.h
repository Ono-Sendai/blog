/*=====================================================================
PostHandlers.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


namespace PostHandlers
{
	void renderShowPostPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info, unsigned int post_i);
	void handleNewPostPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderNewPostPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void handleEditPostPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderConfirmDeletePost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void deletePostPost(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderEditPostPage(DataStore& datastore, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
} 
