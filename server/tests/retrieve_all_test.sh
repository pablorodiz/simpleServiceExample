#!/bin/bash
cp retrieve_all_test.database.txt database.txt
#Launch server
../Debug/server > retrieve_all_test.out 2>retrieve_all_test.err &
PID=`pidof ../Debug/server`
sleep 2
#Send some text to the server
cat retrieve_all_test.in | nc -q 2 127.0.0.1 6423 > retrieve_all_test.client
#Terminate server
disown $PID
kill $PID
#Check server response
if [ -z "`diff retrieve_all_test.out.ok retrieve_all_test.out`" ] && [ -z "`diff retrieve_all_test.client.ok retrieve_all_test.client`" ] && [ -f `cat retrieve_all_test.err` ] 
  then
    echo "Retrieve All test OK"
    rm retrieve_all_test.out retrieve_all_test.err retrieve_all_test.client
  else
    echo "Retrieve All test FAILED"
fi
