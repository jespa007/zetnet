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
SET OPENSSL_VERSION=openssl-openssl-3.0.4
SET OPENSSL_FILE=%OPENSSL_VERSION%.tar.gz

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
:: openssl
::
echo building openssl...

IF NOT EXIST %THIRD_PARTY_BUILD_DIR%\%OPENSSL_VERSION%\NUL (
	tools\7zip\7za.exe x src\3rdparty\%OPENSSL_FILE% -so| tools\7zip\7za.exe x -aoa -si -ttar -o%THIRD_PARTY_BUILD_DIR%
)

IF NOT EXIST %THIRD_PARTY_LIB%\openssl.lib (
	
	cd %THIRD_PARTY_BUILD_DIR%\%OPENSSL_VERSION%
		
	perl Configure VC-WIN64A no-shared PREFIX=%THIRD_PARTY_INSTALL_DIR%
	
	nmake install
	
	IF NOT EXIST %THIRD_PARTY_INCLUDE%\openssh (
		MD %THIRD_PARTY_INCLUDE%\openssh
		copy include\* %THIRD_PARTY_INCLUDE%\openssh
	)
	copy build\Release\* %THIRD_PARTY_LIB%
)

cd %ROOT_DIR%

::------------------------------------------------------------------------------------------------------------------------
::
:: zetnet
::

echo building zetnet...

IF NOT EXIST %THIRD_PARTY_LIB%\zetnet.lib (

	rd build\msvc\zetnet /S /Q
	cmake  -H. -Bbuild\msvc\zetnet -G %CMAKE_GENERATOR%
	msbuild build\msvc\zetnet\zetnet.sln /t:Rebuild /p:Configuration=Release

	copy build\msvc\zetnet\Release\zetnet.exe %BINARY_DIR%
)

echo All OK

