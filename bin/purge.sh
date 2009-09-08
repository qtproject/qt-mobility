#!/bin/bash

make clean
rm config.pri
find . -name "Makefile*" |xargs rm
rm -rf build include install bin/examples
