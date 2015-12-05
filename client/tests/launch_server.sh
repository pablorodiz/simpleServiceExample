#!/bin/bash
#Create example database
cp database.example database.txt
#Launch server
../../server/Debug/server > server.log 2>server.error.log &
PID=`pidof ../../server/Debug/server`
echo "Press any key to terminate server"
read -n 1
#Terminate server
disown $PID
kill $PID
