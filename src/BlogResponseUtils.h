/*=====================================================================
BlogResponseUtils.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include <string>
namespace web { class RequestInfo; }
namespace web { class ReplyInfo; }
class DataStore;


/*=====================================================================
ResponseUtils
-------------------

=====================================================================*/
namespace BlogResponseUtils
{
	const std::string standardHTMLHeader(DataStore& datastore, const web::RequestInfo& request_info, const std::string& page_title);
	const std::string standardHeader(DataStore& datastore, const web::RequestInfo& request_info, const std::string& page_title);

	const std::string standardFooter(DataStore& datastore, const web::RequestInfo& request_info, bool include_email_link);
}




