#!/bin/sh

for j in $(seq 0 10000)
do
	curl "127.0.0.1:6666/simple/network?act=mission_task&itemid=456&name=今天星期五"
done 