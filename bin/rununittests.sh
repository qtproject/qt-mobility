#!/bin/bash

#convenient script to execute all unit tests

shell=`readlink -f "$0"`;
shell_path=`dirname $shell`;

SAVED_PWD=$PWD;
cd $shell_path/../build/tests/Debug/bin
LD_LIBRARY_PATH=$shell_path/../build/Debug/bin:$LD_LIBRARY_PATH

./tst_qserviceinterfacedescriptor
./tst_servicedatabasetest
./tst_servicemetadata
./tst_qservicefilter
./tst_qservicemanager
./tst_qabstractsecuritysession

cd $SAVED_PWD

