#!/bin/bash
#Launch server
../Debug/server > connectivity_test.out 2>connectivity_test.err &
PID=`pidof ../Debug/server`
sleep 2
#Send some text to the server
echo "hello" | nc -q 2 127.0.0.1 6423
#Terminate server
disown $PID
kill $PID
#Check server response
if [ -z "`diff connectivity_test.ok connectivity_test.out`" ] && [ -z "`diff connectivity_test.err connectivity_test.err.ok`" ] 
  then
    echo "Connectivity test OK"
    rm connectivity_test.out connectivity_test.err
  else
    echo "Connectivity test FAILED"
fi
