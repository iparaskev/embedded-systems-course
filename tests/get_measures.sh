#!/bin/bash

percentages=$(cat logs/top_results.log | grep server|  grep -v grep | awk '{print $7}');

for line in $percentages;
do
            measure=$(echo "${line%%\%*}");
            echo $measure >> logs/cpu.log;
done

