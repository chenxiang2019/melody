#!/bin/bash
#genetate json and dot files
# first, installing the bmv2 and p4c, refer to this
# https://www.cnblogs.com/guidoahead/p/12286278.html

filename=$1
file_path=$2
mkdir -p preprocessor/build
p4c-bm2-ss --p4v 16 --p4runtime-files preprocessor/build/${filename}.p4.p4info.txt -o preprocessor/build/${filename}.json ${file_path}

mkdir -p preprocessor/out
p4c-graphs ${file_path} --graphs-dir preprocessor/out

sleep 5
# move the file to the workspace
cp preprocessor/build/${filename}.json analyzer/${filename}.json
cp preprocessor/out/ingress.dot analyzer/${filename}.dot