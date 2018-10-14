#!/bin/bash

percentages=$(cat logs/top_results.log | grep server_loc| awk '{print $8}');

for line in $percentages;
do
            prefix=$(echo "${line%%,*}");
            suffix=$(echo "${line#$prefix,}");
            CPU=$(echo $prefix.$suffix);
            echo $CPU >> logs/cpu.log;
done

