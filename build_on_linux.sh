#!/bin/bash
mkdir -p /robot/build/debian
mkdir -p /robot/output
cd /robot/build/debian && cmake ../../ -DCMAKE_BUILD_TYPE=Release && make clean && make
#ldd /robot/royalserver/battlerobot/output/exe/Linux/robot
#cd /lib/&& tar zcf x86_64-linux-gnu.tar.gz x86_64-linux-gnu && cp x86_64-linux-gnu.tar.gz /robot/runtime-lib/ && rm x86_64-linux-gnu.tar.gz
