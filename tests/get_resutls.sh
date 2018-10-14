#!/bin/bash

IP=$1
PORT=$2

echo -e "0\n\n" | ./bin/client $IP $PORT -t > logs/0.log
echo -e "1\n\n" | ./bin/client $IP $PORT -t > logs/1.log
