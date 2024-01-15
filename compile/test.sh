#!/bin/bash

set -e
echo "run test "
cd src/build/bin
./Test.exe
echo "test finish"
cd -
