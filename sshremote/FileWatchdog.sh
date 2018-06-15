#!/bin/sh
# author BokzBCheung
# date 2018-6-14 17:06:36

# show the capacity of the /dev/vda1 
df -h|awk '/vda1/ {print $5}' > capacity.txt

capacity=$(cat capacity.txt)

# write the info to file
echo "your machine 18's capacity is $capacity !" >capacity.txt

# write the machine to file 
df -h > capacityofmachine.txt
