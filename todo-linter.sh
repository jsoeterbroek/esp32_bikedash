#!/usr/bin/env bash

# Who am I
######################################################################
MYFILENAME="todo-linter.sh"

if [ $# -eq 0 ] || [ $# -gt 1 ];
then 
    echo "Usage:"
    echo "    bash ${MYFILENAME} [file or filepath]"
    echo "You may pass a single file or a filepath. The script will recursively check subdirectories."
    exit 1
fi

search_path=$1

keywords="TODO|FIXME"

# 1. Find all files ending with .py or .sh excepting .env/ directories and
#      print their contents.
# 2. For all of the contents, grep for the keyword in a case insensitive manner, using 
#      the options listed.
# 3. Pipe into perl and insert the warning command.
######################################################################
find "${search_path}" \( -name "*.ino" -or -name ".h" \) -not -path "*.env*" -print0 | xargs -0 egrep --with-filename --line-number --only-matching --ignore-case --no-messages "($keywords).*\$" | perl -p -e "s/($keywords)/ warning: \$1/i"
