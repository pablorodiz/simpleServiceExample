#!/bin/bash
#Launch server
../Debug/server > retrieve_test.out 2>retrieve_test.err &
PID=`pidof ../Debug/server`
sleep 2
#Send some text to the server
cat retrieve_test.in | nc -q 2 127.0.0.1 6423
#Terminate server
disown $PID
kill $PID
#Check server response
#if [ -z "`diff retrieve_test.ok retrieve_test.out`" ] && [ -f `cat retrieve_test.err` ] 
#  then
#    echo "Retrieve test OK"
#    rm retrieve_test.out retrieve_test.err
#  else
#    echo "Retrieve test FAILED"
#fi
