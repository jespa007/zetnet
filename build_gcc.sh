#!/bin/bash

BUILD=""
TARGET_OS=""
HOST=""
ARCH=""
CROSS_PREFIX=""
EXTENSION_EXE=""
CMAKE_GENERATOR="Unix Makefiles"
ROOT_DIR=$PWD
THIRD_PARTY_SOURCES_DIR=$ROOT_DIR"/src/3rdparty"
THIRD_PARTY_BUILD_DIR=$ROOT_DIR"/build/gcc/3rdparty"
THIRD_PARTY_INSTALL_DIR=$ROOT_DIR"/3rdparty/gcc"
BINARY_DIR=$ROOT_DIR"/bin/gcc/release"
TOOLCHAIN=""
TARGET="release"

 
# The third parties
OPENSSL_TAG="openssl-openssl-3.0.4"

OPENSSL_FILE=$OPENSSL_TAG".tar.gz"


EXTENSION_STATIC_LIB=".a"
INSTALLED_DIR_LIBRARY_FILE="lib"
COMMON_CMAKE_OPTIONS=" -DCMAKE_INSTALL_PREFIX:PATH="$THIRD_PARTY_INSTALL_DIR
COMMON_CONFIGURE_OPTIONS=" --disable-shared --enable-static --prefix="$THIRD_PARTY_INSTALL_DIR
CMAKE_CONFIG=($COMMON_CMAKE_OPTIONS)
export CFLAGS="-O2 -I"$THIRD_PARTY_INSTALL_DIR"/include"
export LDFLAGS="-L"$THIRD_PARTY_INSTALL_DIR"/lib"
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$THIRD_PARTY_INSTALL_DIR"/lib/pkgconfig"


if [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]; then
	TOOLCHAIN="gcc"
	EXTENSION_EXE=".exe"
	INSTALLED_DIR_LIBRARY_FILE="bin"
	TOOLCHAIN=$1
	EXTENSION_STATIC_LIB=".lib"

	EXTENSION_STATIC_LIB=".a"
	HOST=x86_64-w64-mingw32
	CROSS_PREFIX=x86_64-w64-mingw32-
	ARCH=x86_64
	BUILD=$HOST
	TARGET_OS=mingw32
	COMMON_CONFIGURE_OPTIONS=$COMMON_CONFIGURE_OPTIONS" --build=$BUILD"
	CMAKE_CONFIG=($COMMON_CMAKE_OPTIONS -DCMAKE_COLOR_MAKEFILE=0xFF -G 'MinGW Makefiles' )

else
	export CFLAGS=$CFLAGS" -fPIC"
fi


echo THIRD_PARTY_INSTALL_DIR=$THIRD_PARTY_INSTALL_DIR
echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
echo LDFLAGS=$LDFLAGS
echo CFLAGS=$CFLAGS
echo THIRD_PARTY_INCLUDE=$THIRD_PARTY_INSTALL_DIR/include


if ! [ -d "$THIRD_PARTY_BUILD_DIR" ] 
then
	mkdir -p $THIRD_PARTY_BUILD_DIR
fi

	
if ! [ -d $THIRD_PARTY_INSTALL_DIR"/lib" ]
then
	mkdir -p $THIRD_PARTY_INSTALL_DIR"/lib"
fi 

cd $THIRD_PARTY_BUILD_DIR


#------------------------------------------------------------------------------------------------------------------------
#
# openssl
#

echo building openssl...

if ! [ -d $THIRD_PARTY_BUILD_DIR"/$OPENSSL_TAG" ] 
then
	tar xf $THIRD_PARTY_SOURCES_DIR"/"$OPENSSL_FILE
fi

cd $OPENSSL_TAG

if ! [ -f $THIRD_PARTY_INSTALL_DIR"/lib/libopenssl"$EXTENSION_STATIC_LIB ] 
then

	rm -rf build

	perl Configure mingw64  --prefix=$THIRD_PARTY_INSTALL_DIR


	if ! [ $? -eq 0 ] 
	then
		exit 1
	fi

	make clean
	make -j2 install

	if ! [ $? -eq 0 ] 
	then
		exit 1
	fi
fi

cd ..

#------------------------------------------------------------------------------------------------------------------------
#
# zetnet
#
echo building zetnet...
cd $ROOT_DIR

if ! [ -f $BINARY_DIR"/libzetnet"$EXTENSION_STATIC_LIB ] 
then

	rm -rf build/gcc/zs_ge
	cmake -H. -Bbuild/gcc/zetnet "${CMAKE_CONFIG[@]}"
	
	if ! [ $? -eq 0 ] 
	then
		exit 1
	fi
	
	make clean -C build/gcc/zetnet/release
	make -C build/gcc/zetnet
		
	if ! [ $? -eq 0 ] 
	then
		exit 1
	fi
fi

#------------------------------------------------------------------------------------------------------------------------

if ! [ "$TARGET_OS" = "mingw32" ]
then

	if [ -d $THIRD_PARTY_INSTALL_DIR"/lib/cmake" ] 
	then
		echo "cleaning ..."
		rm $THIRD_PARTY_INSTALL_DIR"/lib/cmake" -R
	fi
fi

echo "All OK"
