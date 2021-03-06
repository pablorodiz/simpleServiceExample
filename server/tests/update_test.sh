#!/bin/bash
rm -f database.txt
#Launch server
../Debug/server > update_test.out 2>update_test.err &
PID=`pidof ../Debug/server`
sleep 2
#Send some text to the server
cat update_test.in | nc -q 2 127.0.0.1 6423
#Terminate server
disown $PID
kill $PID
#Check server response
cat database.txt | cut -f2,3 -d" " > update_test_database_check.out
if [ -z "`diff update_test.out.ok update_test.out`" ] && [ -z "`diff update_test_database.ok update_test_database_check.out`" ] && [ -f `cat update_test.err` ] 
  then
    echo "Update test OK"
    rm update_test.out update_test.err
  else
    echo "update test FAILED"
fi
rm -f update_test_database_check.out
