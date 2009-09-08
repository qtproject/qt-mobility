#!/bin/bash

make clean
rm -f config.pri
find . -name "Makefile*" |xargs rm
rm -rf build include install bin/examples lib
