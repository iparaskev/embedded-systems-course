#!/bin/bash

IP=$1
PORT=$2;
ID=$3;
rounds=$4;
limit=$5;
sleep_break=$6;
max_sleep=$7;
messages=$8;

# Total sended messages to every user

# Remove the previous log file
rm -f logs/$ID.log
rm -f logs/time.log

# Measure time
time_start=$(date +%s.%N);
for i in `seq 1 $rounds`;
do
        #n_messages=$(echo $(( (RANDOM % $limit) + 2 )));
        n_messages=$(echo $(( limit + 1 )));

        # Counter for checking how many messages send to every user
        counter_0=0;
        counter_1=0;

        # Send the messages
        for message in `seq 2 $n_messages`;
        do
                if [[ $(($message % 2)) == 0 ]];
                then
                        # Make the string for stdin
                        std="$ID\n"
                        end_msg=$(echo $(( counter_0 + messages - 1)))
                        for j in `seq $counter_0 $end_msg`;
                        do
                                std=$(echo "${std}0\n${ID}_${i}_${j}\n")
                        done
                        echo -e $std | ./bin/client $IP $PORT;
                        counter_0=$(echo $((end_msg + 1)))
                else
                        # Make the string for stdin
                        stdin="$ID\n";
                        end_msg=$(echo $(( counter_1 + messages - 1)));
                        for j in `seq $counter_1 $end_msg`;
                        do
                                stdin=$(echo "${stdin}1\n${ID}_${i}_${j}\n");
                        done
                        echo -e $stdin | ./bin/client $IP $PORT;
                        counter_1=$(echo $((end_msg + 1)));
                fi

                sleep_time=$(echo $(( (RANDOM % $sleep_break) )));
                sleep $sleep_time
        done

        echo "${i}_${counter_0}_${counter_1}" >> logs/$ID.log
        
        sleep_time=$(echo $(( (RANDOM % $max_sleep) + 1 )));
        sleep $max_sleep;
done
time_end=$(date +%s.%N);

# Write the time
echo " $time_end - $time_start" | bc >> logs/time.log



