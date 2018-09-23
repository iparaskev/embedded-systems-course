#!/bin/bash

IP=$1;
PORT=$2;
limit=$3;
clients=$4;


# Get the server pid
pid=$(ps | grep server | awk '{print $1}');

# Start the concurrent client processes
wait_pids=();
for i in `seq 2 $clients`;
do
        ./tests/limits.sh $IP $PORT $i $limit &
	wait_pids+=($!);
done

# Write the cpu logs to file
./tests/cpu.sh $pid &

wait ${wait_pids[*]};

# Get the cpu measure pid
pid=$(ps | grep cpu.sh | awk '{print $1}');
kill $pid;

# Write the received messages to files
./tests/get_resutls.sh $IP $PORT

# Validate the send/receive messages
python ./tests/validate_results.py

# Print the mean waiting processes
#./tests/add.sh waiting_counter.log

# Print the mean cpu load
./tests/add.sh cpu.log

