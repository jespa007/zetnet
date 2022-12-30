cmake_minimum_required(VERSION 3.15)
 
project(zetnet)


file(READ "zetnet.h" zetnet_h)

string(REGEX MATCH "ZETNET_VERSION_MAJOR([\t-\ ][0-9])*" _ ${zetnet_h})
string(STRIP "${CMAKE_MATCH_1}" ZETNET_VERSION_MAJOR)

string(REGEX MATCH "ZETNET_VERSION_MINOR([\t-\ ][0-9])*" _ ${zetnet_h})
string(STRIP "${CMAKE_MATCH_1}" ZETNET_VERSION_MINOR)

string(REGEX MATCH "ZETNET_VERSION_PATCH([\t-\ ][0-9])*" _ ${zetnet_h})
string(STRIP "${CMAKE_MATCH_1}" ZETNET_VERSION_PATCH)



MESSAGE("-- Zetnet ${ZETNET_VERSION_MAJOR}.${ZETNET_VERSION_MINOR}.${ZETNET_VERSION_PATCH}" )

set(ZETNET_LIB_NAME "zetnet-${ZETNET_VERSION_MAJOR}-${ZETNET_VERSION_MINOR}-${ZETNET_VERSION_PATCH}")

#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# MACRO UTILS
#


MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)

    FOREACH(HEADER ${${HEADER_LIST}})
        get_filename_component(DIR ${HEADER} DIRECTORY)
        INSTALL(FILES ${HEADER} DESTINATION include/zetnet/${DIR})
    ENDFOREACH(HEADER)

ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)



#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#


include_directories (
	${PROJECT_SOURCE_DIR}
)

message("-- Install directory: " ${CMAKE_INSTALL_PREFIX})

set( POST_NAME "")
set(ZETNET_LIB_SRCS "")


if(MSVC)
	MESSAGE ("Target: MSVC" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS )

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)
	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")	

else()  # g++ / gcc
	SET( CMAKE_C_FLAGS " -D__STDC_CONSTANT_MACROS -pthread -fomit-frame-pointer -pipe -Wall -Wextra -Wstrict-prototypes -pedantic ")
	  
	if (MINGW)
		MESSAGE ("-- Plataform: MINGW" )
    elseif(GNU)
    	MESSAGE ("-- Plataform: GNU" )
    else()
    	MESSAGE ("-- Plataform: Unknown" )
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug" AND EXISTS  ${PROJECT_SOURCE_DIR}/../../memmgr)
		set( POST_NAME "_d")
		SET( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -O0 -g -D__DEBUG__ ")
		message("-- Memmanager: ON")
		SET( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -D__MEMMANAGER__")
		include_directories (
			${PROJECT_SOURCE_DIR}/../../memmgr
		)
	
   		set(ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS} ${PROJECT_SOURCE_DIR}/../../memmgr/memmgr.c)
		
	ELSE()
		set(TARGET "release")
		SET( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -O2 ")
	   	if(UNIX)
			set(CMAKE_C_FLAGS " ${CMAKE_CXX_FLAGS} -fsanitize=address ")
			message("-- Memmanager: -fsanitize=address")
		else()
			message("-- Memmanager: OFF (../../memmgr not exist)")
		endif()		
	ENDIF()
	
	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	

endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	MESSAGE ("-- Architecture: 64bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    MESSAGE ("-- Architecture: 32bit")
endif()

MESSAGE ("-- Target: " ${TARGET})


#
# COMMON PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMPILE PARAMS
#

# Library


set( ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS}

	# util
	util/zn_util.c
	
	# http
	http/zn_http.c

	zetnet.c
)

#-------------------------------------------------------------------------------------------------------------------------------------
#
# LIBRARY/EXECUTABLE PARAMS
#
	
set( LIBRARY_TYPE_CREATION "STATIC" )
if(BUILD_SHARED_LIBS)
	set( LIBRARY_TYPE_CREATION "SHARED" )
endif()

message("-- Library type: " ${LIBRARY_TYPE_CREATION})

#HTTPSERVER TEST executable
set(ZETNET_HTTPSERVER_SRCS ${PROJECT_SOURCE_DIR}/../test/httpserver.c)



# httpserver library
add_library( ${ZETNET_LIB_NAME}${POST_NAME} ${LIBRARY_TYPE_CREATION} ${ZETNET_LIB_SRCS} )
target_link_libraries(${ZETNET_LIB_NAME}${POST_NAME} )

if(MSVC)
	#only for msvc...
	set_target_properties(${ZETNET_LIB_NAME}${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETNET_EXPORTS)
endif()
	
if(MINGW OR MSVC)
	target_link_libraries(${ZETNET_LIB_NAME}${POST_NAME} Ws2_32 )	
endif()


SET(
	HS
	util/ZNIo.h
	util/ZNString.h
	util/ZNPath.h
	util/url.h
	zetnet.h
	TcpServer.h
	HttpHandleClient.h
	HttpRequest.h
	HttpResponse.h
	HttpServer.h

)


INSTALL_HEADERS_WITH_DIRECTORY(HS)

#-----------------------------------------
# EXECUTABLES
add_executable(httpserver${POST_NAME}
	${ZETNET_HTTPSERVER_SRCS}
)


target_link_libraries(httpserver${POST_NAME}  ${ZETNET_LIB_NAME}${POST_NAME} -pthread )	



INSTALL(TARGETS ${ZETNET_LIB_NAME}${POST_NAME}
		ARCHIVE DESTINATION  		${CMAKE_INSTALL_PREFIX}/lib
		RUNTIME DESTINATION  		${CMAKE_INSTALL_PREFIX}/bin
    	LIBRARY DESTINATION 		${CMAKE_INSTALL_PREFIX}/lib
    	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)


