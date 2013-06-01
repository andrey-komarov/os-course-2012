#/bin/bash
ID=`./sender.sh Makefile`
./receiver.sh $ID
