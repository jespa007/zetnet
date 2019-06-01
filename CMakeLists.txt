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
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${srcTemplatePath} to ${destDir}/${templateFile}")
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

IF (NOT  (("${LINK}" STREQUAL "SHARED") OR ("${LINK}" STREQUAL "STATIC")))
   SET ( LINK "SHARED" )
ENDIF()

message("-- Link: " ${LINK})

include_directories (
	${PROJECT_SOURCE_DIR}
)

message("-- Dest.dir: " ${CMAKE_INSTALL_PREFIX})

set(ZETNET_LIB_SRCS "")

if(MSVC)
	MESSAGE ("-- Plataform: MSVC" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS --no-rtti -std=gnu++0x -I -Wall")
   
	if (MINGW)
	
		
		SET( COMMON_PARAMS " ${COMMON_PARAMS} -I$ENV{MINGWDIR}/local32/include " )
	
		MESSAGE ("-- Plataform: MINGW" )
		message("-- Mingw path: $ENV{MINGWDIR} ")
		
		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")
		
		
  
	  	message("-- params: ${COMMON_PARAMS} ")
	  
	    elseif(GNU)
	    	MESSAGE ("-- Plataform: GNU" )
	    else()
	    	MESSAGE ("-- Plataform: Unknown" )
		endif()
	
		IF (MEMMANAGER)
			message("-- Memmanager: ON (Warning execution time could be slow)")
			SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__MEMMANAGER__")
			include_directories (
				../memmanager
		)
		
	   	set(ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS} ../memmanager/MEM_ram.cpp)
		
	ELSE()
		message("-- Memmanager: OFF")
	ENDIF()
	

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

	SET(CMAKE_C_FLAGS " ")
	SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
	

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	
endif()


#
# RELEASE PARAMS
#
#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#

# Library
set( ZETNET_LIB_SRCS ${ZETNET_LIB_SRCS}

	CUri.cpp
	string_utils.cpp
	io_utils.cpp
	path_utils.cpp
	time_utils.cpp
	CServer.cpp
	HttpHandleClient.cpp
	HttpRequest.cpp
	HttpResponse.cpp
	HttpServer.cpp
)

#HTTPSERVER TEST executable
set(ZETNET_DEMO_SRCS ${PROJECT_SOURCE_DIR}/httpserver_demo.cpp)



# httpserver library
add_library( zetnet${POST_NAME} ${LINK} ${ZETNET_LIB_SRCS} )
target_link_libraries(zetnet${POST_NAME} "-Wl,--no-as-needed" )

if(MSVC)
	#only for msvc...
	set_target_properties(zetnet${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETNET_EXPORTS)
endif()
	
if(MINGW OR MSVC)
	target_link_libraries(zetnet${POST_NAME} Ws2_32 )	
endif()


SET(
	HS
	zetnet.h
	CServer.h
	HttpHandleClient.h
	HttpRequest.h
	HttpResponse.h
	HttpServer.h
	CUri.h
	CZetnetUtils.h
	
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
