#!/bin/bash

pwd=${PWD}
cd res/cert
xxd -i cacert.pem > cacert.pem.c 
cd $pwd