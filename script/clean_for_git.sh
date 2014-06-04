#!/bin/bash
rm -rf ../build/* ../bin/* ../log/* ../test/*
# rm -rf ../third_party/lxc/lxc-0.7.5
rm -f ../src/tags
./clean_proxy.sh
./clean_zk.sh
exit 0

