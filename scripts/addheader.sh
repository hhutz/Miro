#!/bin/bash
#
# takes the first argument (which should be a list of files)
# and checks whether they already begin with the file in the second
# argument, if not the file is prepended.
#
# uses .tmp and .tmp-header as a temporary file 
# needs LINES adjusted to the correct size
# relies on /usr/bin/head
#
# no warranty, may delete all you need
#
# (C) 2000 Stefan Sablatnoeg
#
# 11 lines is the static part, avoid including the id and date of cvs
# as it changes, also the authors may vary for different parts
#

function hasheader() {
	RESULT=`/usr/bin/head -${LINES} ${2} | diff -q ${1} -`
}

LINES=11
/usr/bin/head -${LINES} ${2} >.tmp-header
for i in ${1}; do
	echo ${i}
	hasheader .tmp-header ${i}
	case "${RESULT}" in
	"") echo "ignoring file ${i}, has header already"
	;;
	*) echo "changing file ${i}, needs header"
	cat ${2} ${i} >.tmp
	cp -f .tmp ${i}
	;;
	esac	
done
rm -f .tmp
rm -f .tmp-header
