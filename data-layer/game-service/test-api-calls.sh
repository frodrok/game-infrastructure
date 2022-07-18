#!/bin/bash

function get_status() {
	URL=$1
	STATUS=$(curl -s -o /dev/null -w "%{http_code}" $1)
	return $STATUS

}

STATUS=$(curl -s -o /dev/null  -w "%{http_code}" -XGET localhost:8000/)

echo "/ $STATUS"
echo ""

STATUS=$(curl -s -o /dev/null -w "%{http_code}" -XPOST -H"Content-Type: application/json" --data '{"username": "fredrik", "password": "hollinger"}' localhost:8000/account/login)

GOOD=false
if [ $STATUS = "200" ]; then 
	
	GOOD="true" 
else 
	GOOD="false" 
fi

get_status "localhost:8000/"
echo "/ STATUS $?"

echo "/login $STATUS $GOOD"

STATUS=$(curl -s -o /dev/null -w "%{http_code}" -XPOST -H"Content-Type: application/json" --data '{"username": "notfound", "password": "notfound"}' localhost:8000/account/login)

if [ $STATUS = "404" ]; then
	GOOD="true"
else
	GOOD="false"
fi

echo "/login 404 $STATUS $GOOD"
