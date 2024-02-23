#!/bin/bash
docker run -it --rm --name=backbone-linux-builder-gcc -v ${PWD}:/src backbone-linux-builder/gcc /bin/bash
