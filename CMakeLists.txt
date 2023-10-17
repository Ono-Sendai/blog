cmake_minimum_required (VERSION 2.8)

# Set the project name
project("blog")

if(WIN32)	
	MESSAGE("MSVC_VERSION: ${MSVC_VERSION}")
	
	set(INDIGO_VS_VER 2022)
	MESSAGE("INDIGO_VS_VER: ${INDIGO_VS_VER}")
endif()


MESSAGE("
Boolean options: (Set with -DOPTION_NAME=TRUE)

Other options: (Set with -DOPTION_NAME=value)
Unix only:
CMAKE_BUILD_TYPE (valid options: Debug, Release, RelWithDebInfo)
")

if(WIN32)
	MESSAGE("Microsoft Visual C/C++ version: ${MSVC_VERSION}")
endif()

OPTION(INDIGO_USE_ASAN "Build with Address Sanitizer support (Clang only)" FALSE)
if(INDIGO_USE_ASAN)
	MESSAGE("INDIGO_USE_ASAN=TRUE.")
else()
	MESSAGE("INDIGO_USE_ASAN=FALSE")
endif()

OPTION(INDIGO_USE_THREAD_SAN "Build with Thread Sanitizer support (Clang only)" FALSE)
if(INDIGO_USE_THREAD_SAN)
	MESSAGE("INDIGO_USE_THREAD_SAN=TRUE.")
else()
	MESSAGE("INDIGO_USE_THREAD_SAN=FALSE")
endif()

if(NOT WIN32)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "")
		MESSAGE(FATAL_ERROR "ERROR: No build type specified. Use -DCMAKE_BUILD_TYPE to specify a build type.")
	else()
		if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
			if(NOT CMAKE_BUILD_TYPE STREQUAL "Release") 
				if(NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
					MESSAGE(FATAL_ERROR "ERROR: ${CMAKE_BUILD_TYPE} is not a valid build type.")
				endif()
			endif()
		endif()
	endif()

	MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")
endif()

MESSAGE("") # New line


set(GLARE_CORE_LIBS_ENV "$ENV{GLARE_CORE_LIBS}")
MESSAGE("GLARE_CORE_LIBS_ENV: ${GLARE_CORE_LIBS_ENV}")


set(INDIGO_LIBRESSL_VERSION "3.5.2")
set(INDIGO_OPENSSL_VERSION "1.0.2a")


if(WIN32)
	set(libressldir "${GLARE_CORE_LIBS_ENV}/LibreSSL/libressl-${INDIGO_LIBRESSL_VERSION}-x64-vs${INDIGO_VS_VER}-install")
	
	set(openssl_include_dir "${openssldir}/include")
	#set(libressl_include_dir "${libressldir}/include")
else()
	# Linux
	set(openssldir "/usr/include")
	set(libressldir "/usr/include")
	set(openssl_include_dir "/usr/include")
	set(libressl_include_dir "/usr/include")
	
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "$ENV{NICKBLOG_OUTPUT}")
	
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "$ENV{NICKBLOG_OUTPUT}")
	
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "$ENV{NICKBLOG_OUTPUT}/test_builds")
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "$ENV{NICKBLOG_OUTPUT}")
	
endif()

MESSAGE("libressldir: ${libressldir}")
MESSAGE("openssldir: ${openssldir}")
	

set(CURRENT_TARGET nickblog)


set(GLARE_CORE_TRUNK_DIR "$ENV{GLARE_CORE_TRUNK_DIR}")
set(WEBSITE_CORE_DIR "${GLARE_CORE_TRUNK_DIR}/webserver")

MESSAGE("WEBSITE_CORE_DIR: ${WEBSITE_CORE_DIR}")
MESSAGE("NICKBLOG_OUTPUT: $ENV{NICKBLOG_OUTPUT}")


include_directories(
${openssl_include_dir}
#${libressl_include_dir}
${GLARE_CORE_TRUNK_DIR}
${GLARE_CORE_TRUNK_DIR}/utils
${GLARE_CORE_TRUNK_DIR}/networking
${WEBSITE_CORE_DIR}
)

SET(nick_blog_files
src/BlogHandlers.cpp
src/BlogHandlers.h
src/BlogRequestHandler.cpp
src/BlogRequestHandler.h
src/DataStore.cpp
src/DataStore.h
src/LoginHandlers.cpp
src/LoginHandlers.h
src/Page.cpp
src/Page.h
src/PageHandlers.cpp
src/PageHandlers.h
src/Post.cpp
src/Post.h
src/PostHandlers.cpp
src/PostHandlers.h
src/RSSHandlers.cpp
src/RSSHandlers.h
src/site.css
src/User.cpp
src/User.h
src/BlogResponseUtils.cpp
src/BlogResponseUtils.h
src/website.cpp
src/BlogRequestHandler.cpp
src/BlogRequestHandler.h
src/UserWebSession.cpp
src/UserWebSession.h
)

########### Website Core ################
SET(website_core_files
${WEBSITE_CORE_DIR}/Escaping.cpp
${WEBSITE_CORE_DIR}/Escaping.h
${WEBSITE_CORE_DIR}/WebListenerThread.cpp
${WEBSITE_CORE_DIR}/WebListenerThread.h
${WEBSITE_CORE_DIR}/RequestInfo.cpp
${WEBSITE_CORE_DIR}/RequestInfo.h
${WEBSITE_CORE_DIR}/Response.cpp
${WEBSITE_CORE_DIR}/Response.h
${WEBSITE_CORE_DIR}/ResponseUtils.cpp
${WEBSITE_CORE_DIR}/ResponseUtils.h
${WEBSITE_CORE_DIR}/Log.cpp
${WEBSITE_CORE_DIR}/Log.h
${WEBSITE_CORE_DIR}/StressTest.cpp
${WEBSITE_CORE_DIR}/StressTest.h
${WEBSITE_CORE_DIR}/TimeStamp.cpp
${WEBSITE_CORE_DIR}/TimeStamp.h
${WEBSITE_CORE_DIR}/WebsiteExcep.h
${WEBSITE_CORE_DIR}/WorkerThread.cpp
${WEBSITE_CORE_DIR}/WorkerThread.h
${WEBSITE_CORE_DIR}/RequestHandler.cpp
${WEBSITE_CORE_DIR}/RequestHandler.h
${WEBSITE_CORE_DIR}/UnsafeString.cpp
${WEBSITE_CORE_DIR}/UnsafeString.h
${WEBSITE_CORE_DIR}/WebWorkerThreadTests.cpp
${WEBSITE_CORE_DIR}/WebWorkerThreadTests.h
${WEBSITE_CORE_DIR}/StaticAssetManager.cpp
${WEBSITE_CORE_DIR}/StaticAssetManager.h
)


########### Networking ################
SET(networking_files
${GLARE_CORE_TRUNK_DIR}/networking/FractionListener.h
${GLARE_CORE_TRUNK_DIR}/networking/IPAddress.cpp
${GLARE_CORE_TRUNK_DIR}/networking/IPAddress.h
${GLARE_CORE_TRUNK_DIR}/networking/MySocket.cpp
${GLARE_CORE_TRUNK_DIR}/networking/MySocket.h
${GLARE_CORE_TRUNK_DIR}/networking/Networking.cpp
${GLARE_CORE_TRUNK_DIR}/networking/Networking.h
${GLARE_CORE_TRUNK_DIR}/networking/NetworkingTests.cpp
${GLARE_CORE_TRUNK_DIR}/networking/NetworkingTests.h
${GLARE_CORE_TRUNK_DIR}/networking/Packet.cpp
${GLARE_CORE_TRUNK_DIR}/networking/Packet.h
${GLARE_CORE_TRUNK_DIR}/networking/SocketTests.cpp
${GLARE_CORE_TRUNK_DIR}/networking/SocketTests.h
${GLARE_CORE_TRUNK_DIR}/networking/SocketInterface.cpp
${GLARE_CORE_TRUNK_DIR}/networking/SocketInterface.h
${GLARE_CORE_TRUNK_DIR}/networking/TestSocket.cpp
${GLARE_CORE_TRUNK_DIR}/networking/TestSocket.h
${GLARE_CORE_TRUNK_DIR}/networking/TLSSocket.cpp
${GLARE_CORE_TRUNK_DIR}/networking/TLSSocket.h
${GLARE_CORE_TRUNK_DIR}/networking/UDPSocket.cpp
${GLARE_CORE_TRUNK_DIR}/networking/UDPSocket.h
${GLARE_CORE_TRUNK_DIR}/networking/URL.cpp
${GLARE_CORE_TRUNK_DIR}/networking/URL.h
)

########### Utils ################
SET(util_files
${GLARE_CORE_TRUNK_DIR}/utils/AESEncryption.cpp
${GLARE_CORE_TRUNK_DIR}/utils/AESEncryption.h
${GLARE_CORE_TRUNK_DIR}/utils/ArgumentParser.cpp
${GLARE_CORE_TRUNK_DIR}/utils/ArgumentParser.h
${GLARE_CORE_TRUNK_DIR}/utils/BufferInStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/BufferInStream.h
${GLARE_CORE_TRUNK_DIR}/utils/BufferOutStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/BufferOutStream.h
${GLARE_CORE_TRUNK_DIR}/utils/Clock.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Clock.h
${GLARE_CORE_TRUNK_DIR}/utils/Condition.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Condition.h
${GLARE_CORE_TRUNK_DIR}/utils/ConPrint.cpp
${GLARE_CORE_TRUNK_DIR}/utils/ConPrint.h
${GLARE_CORE_TRUNK_DIR}/utils/CycleTimer.cpp
${GLARE_CORE_TRUNK_DIR}/utils/CycleTimer.h
${GLARE_CORE_TRUNK_DIR}/utils/CryptoRNG.cpp
${GLARE_CORE_TRUNK_DIR}/utils/CryptoRNG.h
${GLARE_CORE_TRUNK_DIR}/utils/EventFD.cpp
${GLARE_CORE_TRUNK_DIR}/utils/EventFD.h
${GLARE_CORE_TRUNK_DIR}/utils/Exception.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Exception.h
${GLARE_CORE_TRUNK_DIR}/utils/FileHandle.cpp
${GLARE_CORE_TRUNK_DIR}/utils/FileHandle.h
${GLARE_CORE_TRUNK_DIR}/utils/FileInStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/FileInStream.h
${GLARE_CORE_TRUNK_DIR}/utils/FileOutStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/FileOutStream.h
${GLARE_CORE_TRUNK_DIR}/utils/FileUtils.cpp
${GLARE_CORE_TRUNK_DIR}/utils/FileUtils.h
${GLARE_CORE_TRUNK_DIR}/utils/IncludeWindows.h
${GLARE_CORE_TRUNK_DIR}/utils/InStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/InStream.h
${GLARE_CORE_TRUNK_DIR}/utils/KillThreadMessage.cpp
${GLARE_CORE_TRUNK_DIR}/utils/KillThreadMessage.h
${GLARE_CORE_TRUNK_DIR}/utils/Lock.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Lock.h
${GLARE_CORE_TRUNK_DIR}/utils/Maybe.h
${GLARE_CORE_TRUNK_DIR}/utils/MemMappedFile.cpp
${GLARE_CORE_TRUNK_DIR}/utils/MemMappedFile.h
${GLARE_CORE_TRUNK_DIR}/utils/MessageableThread.cpp
${GLARE_CORE_TRUNK_DIR}/utils/MessageableThread.h
${GLARE_CORE_TRUNK_DIR}/utils/Mutex.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Mutex.h
${GLARE_CORE_TRUNK_DIR}/utils/MemAlloc.cpp
${GLARE_CORE_TRUNK_DIR}/utils/MemAlloc.h
${GLARE_CORE_TRUNK_DIR}/utils/MyThread.cpp
${GLARE_CORE_TRUNK_DIR}/utils/MyThread.h
${GLARE_CORE_TRUNK_DIR}/utils/OpenSSL.cpp
${GLARE_CORE_TRUNK_DIR}/utils/OpenSSL.h
${GLARE_CORE_TRUNK_DIR}/utils/OutStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/OutStream.h
${GLARE_CORE_TRUNK_DIR}/utils/Parser.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Parser.h
${GLARE_CORE_TRUNK_DIR}/utils/Platform.h
${GLARE_CORE_TRUNK_DIR}/utils/PlatformUtils.cpp
${GLARE_CORE_TRUNK_DIR}/utils/PlatformUtils.h
${GLARE_CORE_TRUNK_DIR}/utils/RefCounted.h
${GLARE_CORE_TRUNK_DIR}/utils/Reference.h
${GLARE_CORE_TRUNK_DIR}/utils/SHA256.cpp
${GLARE_CORE_TRUNK_DIR}/utils/SHA256.h
${GLARE_CORE_TRUNK_DIR}/utils/StreamUtils.cpp
${GLARE_CORE_TRUNK_DIR}/utils/StreamUtils.h
${GLARE_CORE_TRUNK_DIR}/utils/StringUtils.cpp
${GLARE_CORE_TRUNK_DIR}/utils/StringUtils.h
${GLARE_CORE_TRUNK_DIR}/utils/Task.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Task.h
${GLARE_CORE_TRUNK_DIR}/utils/TaskManager.cpp
${GLARE_CORE_TRUNK_DIR}/utils/TaskManager.h
${GLARE_CORE_TRUNK_DIR}/utils/TaskRunnerThread.cpp
${GLARE_CORE_TRUNK_DIR}/utils/TaskRunnerThread.h
${GLARE_CORE_TRUNK_DIR}/utils/TaskTests.cpp
${GLARE_CORE_TRUNK_DIR}/utils/TaskTests.h
${GLARE_CORE_TRUNK_DIR}/utils/TestUtils.cpp
${GLARE_CORE_TRUNK_DIR}/utils/TestUtils.h
${GLARE_CORE_TRUNK_DIR}/utils/ThreadManager.cpp
${GLARE_CORE_TRUNK_DIR}/utils/ThreadManager.h
${GLARE_CORE_TRUNK_DIR}/utils/ThreadMessage.cpp
${GLARE_CORE_TRUNK_DIR}/utils/ThreadMessage.h
${GLARE_CORE_TRUNK_DIR}/utils/ThreadMessageSink.cpp
${GLARE_CORE_TRUNK_DIR}/utils/ThreadMessageSink.h
${GLARE_CORE_TRUNK_DIR}/utils/ThreadSafeQueue.h
${GLARE_CORE_TRUNK_DIR}/utils/ThreadSafeRefCounted.h
${GLARE_CORE_TRUNK_DIR}/utils/Timer.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Timer.h
${GLARE_CORE_TRUNK_DIR}/utils/SocketBufferOutStream.cpp
${GLARE_CORE_TRUNK_DIR}/utils/SocketBufferOutStream.h
${GLARE_CORE_TRUNK_DIR}/utils/Base64.cpp
${GLARE_CORE_TRUNK_DIR}/utils/Base64.h
${GLARE_CORE_TRUNK_DIR}/utils/RuntimeCheck.cpp
${GLARE_CORE_TRUNK_DIR}/utils/RuntimeCheck.h
)

SET(double_conversion_files
${GLARE_CORE_TRUNK_DIR}/double-conversion/bignum-dtoa.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/bignum-dtoa.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/bignum.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/bignum.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/cached-powers.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/cached-powers.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/double-to-string.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/double-to-string.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/fast-dtoa.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/fast-dtoa.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/fixed-dtoa.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/fixed-dtoa.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/ieee.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/strtod.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/strtod.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/string-to-double.cc
${GLARE_CORE_TRUNK_DIR}/double-conversion/string-to-double.h
${GLARE_CORE_TRUNK_DIR}/double-conversion/utils.h
)

SOURCE_GROUP(blog FILES ${nick_blog_files})
SOURCE_GROUP(website_core FILES ${website_core_files})
SOURCE_GROUP(networking FILES ${networking_files})
SOURCE_GROUP(utils FILES ${util_files})
SOURCE_GROUP(double_conversion FILES ${double_conversion_files})

add_executable(${CURRENT_TARGET}
${nick_blog_files}
${website_core_files}
${networking_files}
${util_files}
${double_conversion_files}
)

add_definitions(-DTLS_SUPPORT=1 -DUSING_LIBRESSL=1)


if(WIN32)
	add_definitions(-DUNICODE -D_UNICODE)
	add_definitions(/MP)
	
	# Set warning level to 4 - this shows some useful stuff not in /W3 in vs2012.
	add_definitions(/W4)
	
	##### Ignore some warnings #####

#	# 'unreferenced formal parameter'
	add_definitions(/wd4100)

	################################
	
	# Consider some options.
	if(NO_WHOLE_PROGRAM_OPT)
		SET(GL_OPT)
	else()
		SET(GL_OPT "/GL")
	endif()
	
	if(WIN_RUNTIME_STATIC)
		SET(WIN_RUNTIME_OPT "/MT")
		SET(WIN_RUNTIME_OPT_DEBUG "/MTd")
	else()
		SET(WIN_RUNTIME_OPT)
		SET(WIN_RUNTIME_OPT_DEBUG)
	endif()
	
	# Append optimisation flags.
	SET(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG}			${WIN_RUNTIME_OPT_DEBUG} -DBUILD_TESTS=0")
	SET(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE}			${WIN_RUNTIME_OPT} -D_SECURE_SCL=0 /Ox ${GL_OPT} -DNDEBUG /Zi")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO}	${WIN_RUNTIME_OPT} /O2 -D_SECURE_SCL=0 -DNDEBUG -DBUILD_TESTS=0")
	
else() # Linux
	#add_definitions(-D__SSSE3__ -D__NO_AVX__)

	#SET(LINUX_C_CXX_OPTIONS "-Wall -fPIC -pthread -mmmx -msse -msse2 -mssse3")
	SET(LINUX_C_CXX_OPTIONS "-Wall -pthread -std=c++0x")
	
	# Turn on address sanitizer if requested.  See http://code.google.com/p/address-sanitizer/wiki/AddressSanitizer 
	if(INDIGO_USE_ASAN)
		# Also emit frame pointers and debug info (-g)
		SET(LINUX_C_CXX_OPTIONS "${LINUX_C_CXX_OPTIONS} -fsanitize=address -fno-omit-frame-pointer -g")
	endif()
	
	# Turn on thread sanitizer if requested.  See http://clang.llvm.org/docs/ThreadSanitizer.html
	if(INDIGO_USE_THREAD_SAN)
		# Update compiler flags
		SET(LINUX_C_CXX_OPTIONS "${LINUX_C_CXX_OPTIONS} -fsanitize=thread -fPIE -fno-omit-frame-pointer -g")
		# Link flags are set in shared_target_settings
	endif()
		
	SET(COMMON_C_CXX_OPTIONS_DEBUG				"${LINUX_C_CXX_OPTIONS} -g")
	SET(COMMON_C_CXX_OPTIONS_RELEASE			"${LINUX_C_CXX_OPTIONS} -O3 -DNDEBUG")
	SET(COMMON_C_CXX_OPTIONS_RELWITHDEBINFO		"${LINUX_C_CXX_OPTIONS} -O2 -DNDEBUG")

	# Append optimisation flags.
	SET(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG}			${COMMON_C_CXX_OPTIONS_DEBUG} -Wno-reorder -DBUILD_TESTS=0")
	SET(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE}			${COMMON_C_CXX_OPTIONS_RELEASE} -Wno-reorder")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO}	${COMMON_C_CXX_OPTIONS_RELWITHDEBINFO} -Wno-reorder -DBUILD_TESTS=0")
	
	SET(CMAKE_C_FLAGS_DEBUG				"${CMAKE_C_FLAGS_DEBUG}				${COMMON_C_CXX_OPTIONS_DEBUG}")
	SET(CMAKE_C_FLAGS_RELEASE			"${CMAKE_C_FLAGS_RELEASE}			${COMMON_C_CXX_OPTIONS_RELEASE}")
	SET(CMAKE_C_FLAGS_RELWITHDEBINFO	"${CMAKE_C_FLAGS_RELWITHDEBINFO}	${COMMON_C_CXX_OPTIONS_RELWITHDEBINFO}")
endif()


# LibreSSL include path and compiler settings.
if(WIN32)
	# Append LibreSSL paths for the configurations
	SET(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE}			/I\"${libressldir}/include\"")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO}	/I\"${libressldir}/include\"")
	SET(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG}			/I\"${libressldir}-debug/include\"")

	SET(LIBRESSL_LINK_FLAGS_RELEASE			"/LIBPATH:\"${libressldir}/lib\"")
	SET(LIBRESSL_LINK_FLAGS_RELWITHDEBINFO	"/LIBPATH:\"${libressldir}/lib\"")
	SET(LIBRESSL_LINK_FLAGS_DEBUG			"/LIBPATH:\"${libressldir}-debug/lib\"")

	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE			"${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LIBRESSL_LINK_FLAGS_RELEASE}")
	SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO	"${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${LIBRESSL_LINK_FLAGS_RELWITHDEBINFO}")
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG			"${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${LIBRESSL_LINK_FLAGS_DEBUG}")
	
	SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE			"${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${LIBRESSL_LINK_FLAGS_RELEASE}")
	SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO	"${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} ${LIBRESSL_LINK_FLAGS_RELWITHDEBINFO}")
	SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG				"${CMAKE_MODULE_LINKER_FLAGS_DEBUG} ${LIBRESSL_LINK_FLAGS_DEBUG}")
else() # linux
	SET(CMAKE_CXX_FLAGS	"${CMAKE_CXX_FLAGS} -I${libressldir}/include")
endif()


if(WIN32)
	SET(NICKBLOG_LIBS odbc32
		comctl32
		rpcrt4
		ws2_32 # Winsock
		crypto-49
		ssl-52  # ssl and tls libs are needed for using LibreSSL's LibTLS API.
		tls-25
		Bcrypt.lib # Needed for BCryptGenRandom that crypto-xx uses
	)
else()
	# Linux
	SET(LINUX_LIBS -ltls -lssl -lcrypto)

	# Add required Thread sanitizer link flags
	SET(THREAD_SAN_LINK_FLAGS, "")
	if(INDIGO_USE_THREAD_SAN)
		SET(THREAD_SAN_LINK_FLAGS "-fsanitize=thread -pie")
	endif()

	set_target_properties(${CURRENT_TARGET} PROPERTIES LINK_FLAGS "${THREAD_SAN_LINK_FLAGS}")
endif()



target_link_libraries(nickblog
${LINUX_LIBS}
${NICKBLOG_LIBS}
)

