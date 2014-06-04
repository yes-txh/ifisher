#!/bin/bash
function usage()
{
    echo "$0 op"
    echo "op start/stop"
    exit 1
}

if [ $# -ne 1 ]; then
    usage
    exit 1
fi

if [ $1 == "start" ]; then
   cd ../zookeeper
   ./server1/zookeeper-3.4.5/bin/zkServer.sh start
   ./server2/zookeeper-3.4.5/bin/zkServer.sh start
   ./server3/zookeeper-3.4.5/bin/zkServer.sh start
elif [ $1 == "stop" ]; then
   cd ../zookeeper
   ./server1/zookeeper-3.4.5/bin/zkServer.sh stop
   ./server2/zookeeper-3.4.5/bin/zkServer.sh stop
   ./server3/zookeeper-3.4.5/bin/zkServer.sh stop
elif [ $1 == "client" ]; then
   cd ../zookeeper
   ./server1/zookeeper-3.4.5/bin/zkCli.sh -server 127.0.0.1:2182
else 
    echo "unknown op $1"
    exit 1
fi

exit 0
