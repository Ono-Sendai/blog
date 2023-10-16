/*=====================================================================
DataStore.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include "Post.h"
#include "UserWebSession.h"
#include "TimeStamp.h"
#include "Page.h"
#include "User.h"
#include "StaticAssetManager.h"
#include <Mutex.h>
#include <RefCounted.h>
#include <vector>


/*=====================================================================
DataStore
-------------------

=====================================================================*/
class DataStore : public RefCounted
{
public:
	DataStore(const std::string& path);
	~DataStore();

	void loadFromDisk();
	void writeToDisk();

	void updateAssetManagerAndPageURLMap();


	mutable Mutex mutex;

	std::map<std::string, UserWebSessionRef> user_web_sessions; // Map from key to UserWebSession

	std::vector<Post> posts;
	std::vector<Page> pages;
	std::vector<User> users;

	std::string path;

	web::StaticAssetManager static_asset_manager;

	std::map<std::string, int> page_url_map;

	std::string letsencrypt_webroot;
	std::string public_files_dir;
	std::string resources_dir;
};
