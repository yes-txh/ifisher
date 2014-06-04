#!/bin/bash
cd /root/lynn-2.0.0/conf
sed -i '/lynn_version/d' executor.conf
echo $1 >> executor.conf 
cd /root/lynn-2.0.0/bin
killall executor
rm -rf executor
hadoop dfs -copyToLocal /lynn/bin/executor  .
chmod +x executor
nohup ./executor  >/dev/null 2>&1 &
exit
