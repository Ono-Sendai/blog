/*=====================================================================
BlogRequestHandler.h
--------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include <RequestHandler.h>
class DataStore;


/*=====================================================================
BlogRequestHandler
-------------------

=====================================================================*/
class BlogRequestHandler : public web::RequestHandler
{
public:
	BlogRequestHandler();
	virtual ~BlogRequestHandler();

	virtual void handleRequest(const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	virtual void handleWebsocketTextMessage(const std::string& msg, Reference<SocketInterface>& socket, const Reference<WorkerThread>& worker_thread);

	virtual void websocketConnectionClosed(Reference<SocketInterface>& socket, const Reference<WorkerThread>& worker_thread);

	DataStore* data_store;
private:
};


class BlogSharedRequestHandler : public web::SharedRequestHandler
{
public:
	BlogSharedRequestHandler(){}
	virtual ~BlogSharedRequestHandler(){}

	virtual Reference<web::RequestHandler> getOrMakeRequestHandler() // Factory method for request handler.
	{
		BlogRequestHandler* h = new BlogRequestHandler();
		h->data_store = data_store;
		return h;
	}

	DataStore* data_store;
private:
};
