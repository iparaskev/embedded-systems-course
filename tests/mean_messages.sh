#!/bin/bash

its=$1;
msgs_per_it=$2;
time_per_msg=$3;
time_per_it=$4;

mean_msgs=$(echo " $msgs_per_it / 2" | bc -l);
msgs=$(echo " $mean_msgs * $its " | bc -l);
mean_time_msg=$(echo " $time_per_msg / 2" | bc -l);
mean_time_it=$(echo " $time_per_it / 2" | bc -l);
time=$(echo "$msgs*$mean_time_msg + $its*$mean_time_it" | bc -l);
echo -n "Ratio: "
echo  " $msgs / $time" | bc -l
