#! /bin/bash

echo "
@ECHO OFF
cd %~dp0
start $(basename ${EXECUTABLE})
"
