cmake_minimum_required(VERSION 3.15)
 
project(zetnet)

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

macro(configure_files srcDir destDir)
    message(STATUS "Configuring files to ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Copying file ${srcTemplatePath} to ${destDir}/${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    COPYONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

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
	SET( COMMON_C_PARAMS " -D__STDC_CONSTANT_MACROS -pthread -fomit-frame-pointer -pipe -Wall -Wextra -Wstrict-prototypes -pedantic ")
	  
	if (MINGW)
		MESSAGE ("-- Plataform: MINGW" )
    elseif(GNU)
    	MESSAGE ("-- Plataform: GNU" )
    else()
    	MESSAGE ("-- Plataform: Unknown" )
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug")
		set( POST_NAME "_d")
		SET( COMMON_C_PARAMS " ${COMMON_C_PARAMS} -O0 -g -D__DEBUG__ ")
		IF( EXISTS ${PROJECT_SOURCE_DIR}/../../memmgr )
			message("-- Memmanager: ON")
			SET( COMMON_C_PARAMS " ${COMMON_C_PARAMS} -D__MEMMANAGER__")
			include_directories (
				${PROJECT_SOURCE_DIR}/../../memmgr
			)
		
	   		set(ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS} ${PROJECT_SOURCE_DIR}/../../memmgr/memmgr.c)
	   	else()
	   		message("-- Memmanager: OFF (../../memmgr not exist)")
	   	endif()
		
	ELSE()
		set(TARGET "release")
		SET( COMMON_C_PARAMS " ${COMMON_C_PARAMS} -O2 ")
		message("-- Memmanager: OFF")
	ENDIF()
	
	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	
	SET(CMAKE_C_FLAGS   " ${COMMON_C_PARAMS}  " )
	
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

	util/zn_mem.c
	util/zn_url.c
	util/zn_str.c
	util/zn_list.c
	util/zn_file.c
	util/zn_dir.c
	util/zn_path.c
	TcpServer.c
	http/HttpParamValue.c
	http/HttpHandleClient.c
	http/HttpRequest.c
	http/HttpResponse.c
	http/HttpServer.c
	http/HttpServerMPS.c
	TcpUtils.c
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
add_library( zetnet${POST_NAME} ${LIBRARY_TYPE_CREATION} ${ZETNET_LIB_SRCS} )
target_link_libraries(zetnet${POST_NAME} )

if(MSVC)
	#only for msvc...
	set_target_properties(zetnet${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETNET_EXPORTS)
endif()
	
if(MINGW OR MSVC)
	target_link_libraries(zetnet${POST_NAME} Ws2_32 )	
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


target_link_libraries(httpserver${POST_NAME}  zetnet${POST_NAME} -pthread )	



INSTALL(TARGETS zetnet${POST_NAME}
		ARCHIVE DESTINATION  		${CMAKE_INSTALL_PREFIX}/lib
		RUNTIME DESTINATION  		${CMAKE_INSTALL_PREFIX}/bin
    	LIBRARY DESTINATION 		${CMAKE_INSTALL_PREFIX}/lib
    	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)

#install other resources
configure_files(${PROJECT_SOURCE_DIR}/test/ 				  			${CMAKE_BINARY_DIR}/${TARGET}/)
configure_files(${PROJECT_SOURCE_DIR}/test/www/assets/bootstrap/css 	${CMAKE_BINARY_DIR}/${TARGET}/assets/bootstrap/css)
configure_files(${PROJECT_SOURCE_DIR}/test/www/assets/bootstrap/font 	${CMAKE_BINARY_DIR}/${TARGET}/assets/bootstrap/font)
configure_files(${PROJECT_SOURCE_DIR}/test/www/assets/bootstrap/js 		${CMAKE_BINARY_DIR}/${TARGET}/assets/bootstrap/js)
configure_files(${PROJECT_SOURCE_DIR}/test/www/assets/jquery/js 		${CMAKE_BINARY_DIR}/${TARGET}/assets/jquery/js)
