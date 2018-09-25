#!/bin/bash

IP=$1;
PORT=$2;
limit=$3;
clients=$(echo $(( $4 + 1 )));
time_interval=$5;

# Clear the logs
rm -f logs/*;

# Start the concurrent client processes
wait_pids=();
for i in `seq 2 $clients`;
do
        ./tests/limits.sh $IP $PORT $i $limit $time_interval &
	wait_pids+=($!);
done

# Write the cpu logs to file
ssh root@192.168.1.1 'top' > logs/top_results.log &
fg %ssh


wait ${wait_pids[*]};

echo "Done sending."

# Get the cpu measure pid
./tests/get_measures.sh

# Get pid of top
top_pid=$(ps | grep ssh | awk '{print $1}');
kill $top_pid;

# Write the received messages to files
./tests/get_resutls.sh $IP $PORT

# Validate the send/receive messages
python ./tests/validate_results.py

# Print the mean waiting processes
#./tests/add.sh waiting_counter.log

# Print the mean cpu load
./tests/add.sh logs/cpu.log

