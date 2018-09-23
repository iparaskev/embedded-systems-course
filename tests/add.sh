#!/bin/bash

values=$(cat $1);
range=$(cat $1 | wc -l);

sum=0;
for i in $values;
do
        sum=$(echo " $sum + $i " | bc -l);
done

printable=$(echo "${1%%.*}")
echo -n "Mean $printable: "
echo " $sum / $range" | bc -l 

