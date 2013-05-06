#!/bin/bash
COUNTER=`wget -q -O - http://greenhouse.endofinternet.org/greenhouse/status | python -c 'import json,sys;obj=json.load(sys.stdin);print obj["id"]'`
let "COUNTER_LAST = COUNTER - 1"

while [ "$COUNTER" -gt "$COUNTER_LAST" ];
do
  COUNTER_LAST=$COUNTER
  JSON_MSG=`wget -q -O - http://greenhouse.endofinternet.org/greenhouse/status`
  echo $JSON_MSG
  COUNTER=`echo "$JSON_MSG" | python -c 'import json,sys;obj=json.load(sys.stdin);print obj["id"]'`
  sleep 5
done
