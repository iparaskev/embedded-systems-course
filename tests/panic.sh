#!/bin/bash

pids=$(pgrep -u iasonas -l | grep limits | cut -d ' ' -f 1);
for pid in $pids;
do
        kill $pid;
done

