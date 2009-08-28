#!/bin/bash

make clean
find . -name "Makefile*" |xargs rm
rm -rf build
