#!/bin/bash
#由于Windows上默认使用的编码格式是GBK，这个脚本可以递归的转换GBK 到 utf-8 如果不能转换则说明该文件格式本身有问题  即不是GBK格式
rm failed.txt
for i in `find ./  -type f -name "*.cpp"` ; do 
	(iconv  -f GBK -t utf-8 $i > ${i}.tmp && mv ${i}.tmp $i )||(rm ${i}.tmp &&(printf "convert %s failed\n" ${i})&& (echo ${i} >> failed.txt)) ; done	
for i in `find ./  -type f -name "*.h"` ; do 
	(iconv  -f GBK -t utf-8 $i > ${i}.tmp && mv ${i}.tmp $i )||(rm ${i}.tmp &&(printf "convert %s failed\n" ${i})&& (echo ${i} >> failed.txt)) ; done	

