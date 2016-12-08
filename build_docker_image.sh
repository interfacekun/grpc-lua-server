#!/bin/bash
cd `dirname "$0"`
DIR=`pwd`

#build the component on linux vm
docker run --rm --name robotbuild -v `pwd`:/robot  debian-dev-boost:8.5   /bin/bash /robot/build_on_linux.sh

#create the docker image
docker build --no-cache --rm=true -t robot .

echo "Building Finished"
