#!/bin/bash
COUNTER=`wget -q -O - http://192.168.0.173/greenhouse/counter`
let "COUNTER_LAST = COUNTER - 1"

while [ "$COUNTER" -gt "$COUNTER_LAST" ];
do
  COUNTER_LAST=$COUNTER
  COUNTER=`wget -q -O - http://192.168.0.173/greenhouse/counter`
  echo $COUNTER
done
