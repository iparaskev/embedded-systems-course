#!/bin/bash

# Write
while [ 1 ]
do
	CPU=$(top -b -n1 | grep server_loc | awk '{print $9}');
	prefix=$(echo "${CPU%%,*}");
	suffix=$(echo "${CPU#$prefix,}");
	CPU=$(echo $prefix.$suffix);
	#echo $prefix
	echo $CPU >> logs/cpu.log

	sleep_time=$(echo $(( (RANDOM % 2) )));
	sleep $sleep_time;
done
