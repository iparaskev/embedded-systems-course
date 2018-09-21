#!/bin/bash

IP=$1;
PORT=$2;
limit=$3;

./tests/limits.sh $IP $PORT a $limit > a.log &
./tests/limits.sh $IP $PORT b $limit > b.log &
./tests/limits.sh $IP $PORT c $limit > c.log &
./tests/limits.sh $IP $PORT d $limit > d.log &
./tests/limits.sh $IP $PORT e $limit > e.log &

