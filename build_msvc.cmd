@echo off
SET ROOT_DIR=%~dp0
SET THIRD_PARTY_SOURCES_DIR=%ROOT_DIR%src\3rdparty
SET THIRD_PARTY_BUILD_DIR=%ROOT_DIR%build\msvc\3rdparty
SET THIRD_PARTY_INSTALL_DIR=%ROOT_DIR%3rdparty\msvc
SET THIRD_PARTY_INCLUDE=%THIRD_PARTY_INSTALL_DIR%\include
SET THIRD_PARTY_LIB=%THIRD_PARTY_INSTALL_DIR%\lib
SET BINARY_DIR=%ROOT_DIR%bin\msvc\release
SET TARGET=release
 
:: The third parties
SET SDL2_VERSION=SDL2-2.0.14
SET LIBXMP_VERSION=libxmp-lite-4.5.0
SET ZETSCRIPT_VERSION=zetscript-2.0.0

SET SDL2_FILE=%SDL2_VERSION%.tar.gz
SET LIBXMP_FILE=%LIBXMP_VERSION%.tar.gz
SET ZETSCRIPT_FILE=%ZETSCRIPT_VERSION%.tar.gz

:: Detect which Visual Studio has installed
IF NOT "%VS160COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 16 2019"
)ELSE IF NOT "%VS150COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 15 2017"
)ELSE IF NOT "%VS140COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 14 2015"
) ELSE (
	echo.
	echo Cannot detect Visual Studio environment vars through "Visual Studio Developer Command Prompt". The environment should set one of the followings environment variables,
	echo.
	echo  VS140COMNTOOLS: Environment variable for Visual Studio 2015
	echo  VS150COMNTOOLS: Environment variable for Visual Studio 2017
	echo  VS160COMNTOOLS: Environment variable for Visual Studio 2019
	echo. 
	echo Other Visual Studio versions are not supported
	echo. 
	PAUSE
	EXIT
)


echo -- 3rdparty dest path: %THIRD_PARTY_INSTALL_DIR%
echo -- CMake generator: %CMAKE_GENERATOR%

IF NOT EXIST %THIRD_PARTY_INCLUDE% (
	MD %THIRD_PARTY_INCLUDE%
)

IF NOT EXIST %THIRD_PARTY_LIB% (
	MD %THIRD_PARTY_LIB%
)

IF NOT EXIST %BINARY_DIR% (
	MD %BINARY_DIR%
)

cd %ROOT_DIR%

::------------------------------------------------------------------------------------------------------------------------
::
:: SDL2
::
echo building SDL2...

IF NOT EXIST %THIRD_PARTY_BUILD_DIR%\%SDL2_VERSION%\NUL (
	tools\7zip\7za.exe x src\3rdparty\%SDL2_FILE% -so| tools\7zip\7za.exe x -aoa -si -ttar -o%THIRD_PARTY_BUILD_DIR%
)

IF NOT EXIST %THIRD_PARTY_LIB%\SDL2.lib (
	cd %THIRD_PARTY_BUILD_DIR%\%SDL2_VERSION%

	rd build /S /Q
	cmake  -H. -Bbuild -G %CMAKE_GENERATOR% -DBUILD_SHARED_LIBS:BOOL=OFF -DFORCE_STATIC_VCRT:BOOL=ON
	msbuild build\SDL2.sln /t:Rebuild /p:Configuration=Release
	
	
	IF NOT EXIST %THIRD_PARTY_INCLUDE%\SDL2 (
		MD %THIRD_PARTY_INCLUDE%\SDL2
		copy include\* %THIRD_PARTY_INCLUDE%\SDL2
	)
	
	copy build\Release\* %THIRD_PARTY_LIB%
)

cd %ROOT_DIR%


::------------------------------------------------------------------------------------------------------------------------
::
:: LIBXMP
::
echo building libxmp...

IF NOT EXIST %THIRD_PARTY_BUILD_DIR%\%LIBXMP_VERSION%\NUL (
	tools\7zip\7za.exe x src\3rdparty\%LIBXMP_FILE% -so| tools\7zip\7za.exe x -aoa -si -ttar -o%THIRD_PARTY_BUILD_DIR%
)

IF NOT EXIST %THIRD_PARTY_LIB%\libxmp-lite.lib (
	cd %THIRD_PARTY_BUILD_DIR%\%LIBXMP_VERSION%

	nmake /f Makefile.vc clean
	nmake /f Makefile.vc
		
	copy include\libxmp-lite\xmp.h %THIRD_PARTY_INCLUDE%\xmp.h
	
	copy libxmp-lite.lib %THIRD_PARTY_LIB%
)

cd %ROOT_DIR%

::------------------------------------------------------------------------------------------------------------------------
::
:: zetscript
::
echo building zetscript...

IF NOT EXIST %THIRD_PARTY_BUILD_DIR%\%ZETSCRIPT_VERSION%\NUL (
	tools\7zip\7za.exe x src\3rdparty\%ZETSCRIPT_FILE% -so| tools\7zip\7za.exe x -aoa -si -ttar -o%THIRD_PARTY_BUILD_DIR%
)

IF NOT EXIST %THIRD_PARTY_LIB%\zetscript.lib (
	cd %THIRD_PARTY_BUILD_DIR%\%ZETSCRIPT_VERSION%

	rd build /S /Q
	cmake  -H. -Bbuild -G %CMAKE_GENERATOR% -DBUILD_SHARED_LIBS:BOOL=OFF -DSUIT_TEST:BOOL=false
	msbuild build\zetscript.sln /t:Rebuild /p:Configuration=Release
	
	
	IF NOT EXIST %THIRD_PARTY_INCLUDE%\zetscript (
		MD %THIRD_PARTY_INCLUDE%\zetscript
		xcopy src\*.h %THIRD_PARTY_INCLUDE%\zetscript /s
		xcopy src\*.tcc %THIRD_PARTY_INCLUDE%\zetscript /s
		
	)
	
	copy build\Release\*.lib %THIRD_PARTY_LIB%
)

cd %ROOT_DIR%

::------------------------------------------------------------------------------------------------------------------------
::
:: zg_ge
::

echo building zs_ge...

IF NOT EXIST %BINARY_DIR%\zs_ge.exe (

	rd build\msvc\zs_ge /S /Q
	cmake  -H. -Bbuild\msvc\zs_ge -G %CMAKE_GENERATOR%
	msbuild build\msvc\zs_ge\zs_ge.sln /t:Rebuild /p:Configuration=Release

	copy build\msvc\zs_ge\Release\zs_ge.exe %BINARY_DIR%
)


