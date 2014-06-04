#!/bin/bash
function usage()
{
    echo "$0 num"
    exit 1
}

if [ $# -ne 1 ]; then
    usage
    exit 1
fi

cd ../zookeeper
mkdir -p server1/data server1/dataLog server1/logs
mkdir -p server2/data server2/dataLog server2/logs
mkdir -p server3/data server3/dataLog server3/logs

tar -xvzf zookeeper-3.4.5.tar.gz
cp -r zookeeper-3.4.5 server1
cp -r zookeeper-3.4.5 server2
cp -r zookeeper-3.4.5 server3

echo "tickTime=2000
initLimit=5
syncLimit=2
server.1=127.0.0.1:2888:3888
server.2=127.0.0.1:2889:3889
server.3=127.0.0.1:2890:3890"
>> zoo.cfg

cp zoo.cfg server1/zookeeper-3.4.5/conf/zoo.cfg
echo "dataDir=./server1/data" >> server1/zookeeper-3.4.5/conf/zoo.cfg
echo "dataLogDir=./server1/dataLog" >> server1/zookeeper-3.4.5/conf/zoo.cfg
echo "clientPort=2181" >> server1/zookeeper-3.4.5/conf/zoo.cfg
cp zoo.cfg server2/zookeeper-3.4.5/conf/zoo.cfg
echo "dataDir=./server2/data" >> server2/zookeeper-3.4.5/conf/zoo.cfg
echo "dataLogDir=./server2/dataLog" >> server2/zookeeper-3.4.5/conf/zoo.cfg
echo "clientPort=2182" >> server2/zookeeper-3.4.5/conf/zoo.cfg
cp zoo.cfg server3/zookeeper-3.4.5/conf/zoo.cfg
echo "dataDir=./server3/data" >> server3/zookeeper-3.4.5/conf/zoo.cfg
echo "dataLogDir=./server3/dataLog" >> server3/zookeeper-3.4.5/conf/zoo.cfg
echo "clientPort=2183" >> server3/zookeeper-3.4.5/conf/zoo.cfg

echo 1 >> server1/data/myid
echo 2 >> server2/data/myid
echo 3 >> server3/data/myid

exit 0

