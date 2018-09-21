#!/bin/bash

IP=$1
PORT=$2;
ID=$3;
limit=$4;
max_sleep=10;

# Total sended messages to every user
total_0=0;
total_1=0;
for i in `seq 1 10`;
do
        n_messages=$(echo $(( (RANDOM % $limit) + 3 )));

        # Counter for checking how many messages send to every user
        counter_0=0;
        counter_1=0;

        # Send the messages
        for message in `seq 2 $n_messages`;
        do
                if [[ $(($message % 2)) == 0 ]];
                then
                        echo -e "$message\n0\n${ID}_${i}_${message}" \
                                 | ./bin/client $IP $PORT;
                        counter_0=$(echo $(( counter_0 + 1 )));
                else
                        echo -e "$message\n1\n${ID}_${i}_${message}" \
                                | ./bin/client $IP $PORT;
                        counter_1=$(echo $(( counter_1 + 1 )));
                fi

                sleep_time=$(echo $(( (RANDOM % 3) )));
                sleep $sleep_time
        done

        echo "Iteration: $i Sended $counter_0 messages to user 0"\
             "and $counter_1 messages to user 1."
        
        # Update total messages
        total_0=$(echo $(( total_0 + counter_0 )));
        total_1=$(echo $(( total_1 + counter_1 )));

        sleep_time=$(echo $(( (RANDOM % $max_sleep) + 1 )));
        sleep $sleep_time;
done

echo "Total messages from $ID to 0 $total_0, to 1 $total_1"


