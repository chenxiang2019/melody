#!/bin/bash
if [ $# -lt 2 ]; then
    echo "P4 file dir:" $1
    echo "phv file: dir" $2
    exit 1
fi

arg1=$1
arg2=$2

# 使用正则表达式提取文件名
if [[ $arg1 =~ /([^/]+)\. ]]; then
    filename="${BASH_REMATCH[1]}"
fi

file_path=$1
# 运行 gen.sh
./preprocessor/gen.sh "$filename" "$file_path"

# 运行 melody
./melody "analyzer/${filename}.dot" "analyzer/${filename}.json" "$arg2" > output.txt