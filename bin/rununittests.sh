#!/bin/bash

#convenient script to execute all unit tests

shell=`readlink -f "$0"`;
shell_path=`dirname $shell`;

SAVED_PWD=$PWD;
cd $shell_path/../build/tests/Debug/bin
LD_LIBRARY_PATH=$shell_path/../build/Debug/bin:$LD_LIBRARY_PATH

./tst_qgeoareamonitor
./tst_qgeocoordinate
./tst_qgeopositioninfo
./tst_qgeopositioninfosource
./tst_qgeosatelliteinfo
./tst_qgeosatelliteinfosource
./tst_qlocationutils
./tst_qnmeapositioninfosource

cd $SAVED_PWD

