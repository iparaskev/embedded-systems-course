#!/bin/bash

IP=$1;
if [ $IP = '-h' ]
then
        echo "Arguments"
        echo "IP PORT clients iterations msgs_per_it sleep_per_msg sleep_per_it"\
              " messages"
        exit 0
fi

# Read all the arguments.
PORT=$2;
clients=$(echo $(( $3 + 1 )));
its=$4;
msgs_per_it=$5;
time_per_msg=$6;
time_interval=$7;
messages=$8;

# Clear the logs
rm -f logs/*;

# Write the cpu logs to file
./tests/cpu.sh &

# Start the concurrent client processes
wait_pids=();
for i in `seq 2 $clients`;
do
        ./tests/limits.sh $IP\
                          $PORT\
                          $i\
                          $its\
                          $msgs_per_it\
                          $time_per_msg\
                          $time_interval\
                          $messages\
                          &
	    wait_pids+=($!);
done

wait ${wait_pids[*]};

echo "Done sending."

# Get pid of top
top_pid=$(ps | grep cpu.sh | awk '{print $1}');
kill $top_pid;

# Write the received messages to files
./tests/get_resutls.sh $IP $PORT

# Validate the send/receive messages
python ./tests/validate_results.py

# Print the mean cpu load
./tests/add.sh logs/cpu.log

