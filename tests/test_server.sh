#!/bin/bash

# Send multiple concurrent messages to user 0 and 1
PORT=$1
N=$2
for i in `seq 2 $N`;
do
	if [[ $(($i % 2)) == 0 ]];
	then
		echo -e "$i\n0\na$i" | ./bin/client 127.0.0.1 $PORT;
	else
		echo -e "$i\n1\nb$i" | ./bin/client 127.0.0.1 $PORT;
	fi
done


