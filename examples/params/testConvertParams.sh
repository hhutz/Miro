#!/bin/bash
export PATH=../../../i686_linux_gcc4.1/bin:$PATH

echo \> convertParams example.xml fromxml.xml
convertParams example.xml fromxml.xml
echo

echo \> convertParams example.json fromjson.xml
convertParams example.json fromjson.xml
echo

echo \> convertParams example.xml fromxml.json
convertParams example.xml fromxml.json
echo
