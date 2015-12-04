#!/bin/bash
#Launch server
../Debug/server > retrieve_all_test.out 2>retrieve_all_test.err &
PID=`pidof ../Debug/server`
sleep 2
#Send some text to the server
cat retrieve_all_test.in | nc -q 2 127.0.0.1 6423
#Terminate server
disown $PID
kill $PID
#Check server response
#if [ -z "`diff retrieve_all_test.ok retrieve_all_test.out`" ] && [ -f `cat retrieve_all_test.err` ] 
#  then
#    echo "Retrieve All test OK"
#    rm retrieve_all_test.out retrieve_all_test.err
#  else
#    echo "Retrieve All test FAILED"
#fi
