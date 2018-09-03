#!/bin/bash

# Verify cloc is installed

FOUND_CLOC=$(which cloc | wc -l)
if [ ${FOUND_CLOC} -eq 0 ]; then
	echo "You need to install cloc!"
	exit 1
fi

cloc ../../src --exclude-list-file=exclude-list.txt --by-file | tee detailed_report.txt

cloc ../../src --exclude-list-file=exclude-list.txt | tee summary_report.txt
