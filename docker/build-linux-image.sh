#!/bin/bash
docker container ls -a |grep backbone-linux-builder-gcc|awk '{print $1}'|xargs docker rm -f
docker images|grep backbone-linux-builder/gcc|awk '{print $3}'|xargs docker rmi -f
docker images|grep none|awk '{print $3}'|xargs docker rmi -f
docker build -t backbone-linux-builder/gcc -f dockerfile-builder-linux-gcc .
