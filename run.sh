#!/bin/bash

 if [ -L robot ] ; then
     rm robot
 fi
 linux=`uname -a|grep -i linux`
 if [ "$linux" == "" ]; then
     ln -s output/bin/OSX/robot robot
     ./robot 10 60000
 else
     ln -s output/bin/Linux/robot robot
     ./robot 10 60000
 fi
