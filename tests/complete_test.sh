#!/bin/bash

IP=$1;
PORT=$2;
limit=$3;

./tests/limits.sh $IP $PORT a $limit &
./tests/limits.sh $IP $PORT b $limit &
./tests/limits.sh $IP $PORT c $limit &
./tests/limits.sh $IP $PORT d $limit &
./tests/limits.sh $IP $PORT e $limit &
./tests/limits.sh $IP $PORT f $limit &
./tests/limits.sh $IP $PORT g $limit &
./tests/limits.sh $IP $PORT h $limit &
./tests/limits.sh $IP $PORT i $limit &
./tests/limits.sh $IP $PORT j $limit &

wait

./tests/get_resutls.sh $IP $PORT
python ./tests/validate_results.py

