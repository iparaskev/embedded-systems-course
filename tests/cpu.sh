#!/bin/bash

pid=$1;

# Clear previous log file
rm -f cpu.log;

# Write
while [ 1 ]
do
	CPU=$(top -b -n1 -p $pid | grep server | awk '{print $9}');
	#prefix=$(echo "${CPU%%,*}");
	#suffix=$(echo "${CPU#$prefix,}");
	#CPU=$(echo $prefix.$suffix);
	#echo $prefix
	echo $CPU >> cpu.log

	sleep_time=$(echo $(( (RANDOM % 2) )));
	sleep $sleep_time;
done
