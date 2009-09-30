#!/bin/bash

#convenient script to execute all unit tests

shell=`readlink -f "$0"`;
shell_path=`dirname $shell`;

SAVED_PWD=$PWD;
if [ ! -d "$shell_path/../build/tests/bin" ]; then
    echo -e 'Unit tests have not been built.'
    echo -e 'Please run configure with the -tests switch.'
    exit
fi
cd $shell_path/../build/tests/bin

LD_LIBRARY_PATH=$shell_path/../lib:$LD_LIBRARY_PATH

#service framework
./tst_qserviceinterfacedescriptor
./tst_servicedatabase
./tst_databasemanager
./tst_servicemetadata
./tst_qservicefilter
./tst_qservicemanager
./tst_qabstractsecuritysession

#BearerManagement
./tst_qnetworkconfiguration
./tst_qnetworkconfigurationmanager
./tst_qnetworksession

#Location
./tst_qgeoareamonitor
./tst_qgeocoordinate
./tst_qgeopositioninfo
./tst_qgeopositioninfosource
./tst_qgeosatelliteinfo
./tst_qgeosatelliteinfosource
./tst_qlocationutils
./tst_qnmeapositioninfosource

#Publish and Subscribe
./tst_qmallocpool
./tst_qpacket
./tst_qpacketprotocol
./tst_qvaluespace
./tst_qvaluespaceobject
./tst_qvaluespaceitem
./tst_qsystemreadwritelock
./tst_qsystemreadwritelock_oop

#QSystemInformation
./tst_qsystemdeviceinfo
./tst_qsystemdisplayinfo
./tst_qsysteminfo
./tst_qsystemstorageinfo
./tst_qsystemnetworkinfo
./tst_qsystemscreensaver

cd $SAVED_PWD

