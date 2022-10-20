#!/bin/bash
docker stop gdb
docker rm gdb

if [ -z $SO_IMAGE ]; then
    SO_IMAGE='agodio/itba-so:1.0'
fi

docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name gdb $SO_IMAGE
docker stop gdb
docker rm gdb
