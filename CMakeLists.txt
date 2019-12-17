cmake_minimum_required(VERSION 2.8)
 
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

set(ZETNET_LIB_SRCS "")

if(MSVC)


else() 

	set(TARGET_FLAGS " -O2 ")
	set( POST_NAME "")


	IF ( "${TARGET}" STREQUAL "debug")
		
		set( POST_NAME "_d")	
		
		set(TARGET_FLAGS " -O0 ")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -g -D__DEBUG__ ")
	ENDIF ()

	if ( "${MEMMANAGER}" STREQUAL "ON")
		message("-- Memmanager: ON (Warning execution time could be slow)")
		SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__MEMMGR__")
		include_directories (
			../../lib/memmgr
		)
		
	   	set(ZG_CORE_LIB_SRCS ${ZETNET_LIB_SRCS} ../../lib/memmgr/memmgr.c)
	   		
	else()
		MESSAGE ("-- Memmanager: OFF " )
	endif()

endif()



#
# COMMON PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# RELEASE PARAMS
#


IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "release" )
ENDIF ()
 
 
set( POST_NAME "")
IF ( "${TARGET}" STREQUAL "debug")
	set( POST_NAME "_d")
ENDIF()
 
MESSAGE ("-- Target : " ${TARGET})

IF ( "${TARGET}" STREQUAL "debug")
	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__DEBUG__")
ENDIF()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

	MESSAGE ("-- Toolchain : GNU")

	set(TARGET_FLAGS " -O2 ")
	IF ( "${TARGET}" STREQUAL "debug")
		set(TARGET_FLAGS " -O0 ")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer ")
	ENDIF ()

endif()

SET(CMAKE_C_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )

#Redefine output dir ...
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

#
# RELEASE PARAMS
#
#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#

# Library
set( ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS}

	util/ZNUrl.c
	util/ZNString.c
	util/ZNIo.c
	util/ZNPath.c
	TcpServer.c
	HttpHandleClient.c
	HttpRequest.c
	HttpResponse.c
	HttpServer.c
)

#HTTPSERVER TEST executable
set(ZETNET_DEMO_SRCS ${PROJECT_SOURCE_DIR}/httpserver_demo.c)



# httpserver library
add_library( zetnet${POST_NAME} STATIC ${ZETNET_LIB_SRCS} )
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
add_executable(httpserver_demo${POST_NAME}
	${ZETNET_DEMO_SRCS}
)


target_link_libraries(httpserver_demo${POST_NAME}  zetnet${POST_NAME} -pthread )	



INSTALL(TARGETS zetnet${POST_NAME}
		ARCHIVE DESTINATION  		${CMAKE_INSTALL_PREFIX}/lib
		RUNTIME DESTINATION  		${CMAKE_INSTALL_PREFIX}/bin
    	LIBRARY DESTINATION 		${CMAKE_INSTALL_PREFIX}/lib
    	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)

#install other resources
configure_files(${PROJECT_SOURCE_DIR}/test/ 				  	${PROJECT_SOURCE_DIR}/bin/${TARGET}/)
configure_files(${PROJECT_SOURCE_DIR}/test/assets/bootstrap/css 	${PROJECT_SOURCE_DIR}/bin/${TARGET}/assets/bootstrap/css)
configure_files(${PROJECT_SOURCE_DIR}/test/assets/bootstrap/font 	${PROJECT_SOURCE_DIR}/bin/${TARGET}/assets/bootstrap/font)
configure_files(${PROJECT_SOURCE_DIR}/test/assets/bootstrap/js 	${PROJECT_SOURCE_DIR}/bin/${TARGET}/assets/bootstrap/js)
configure_files(${PROJECT_SOURCE_DIR}/test/assets/jquery/js 		${PROJECT_SOURCE_DIR}/bin/${TARGET}/assets/jquery/js)
