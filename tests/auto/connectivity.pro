include(../../staticconfig.pri)

requires(contains(mobility_modules,connectivity))

TEMPLATE = subdirs

!contains(S60_VERSION, 3.1):!contains(S60_VERSION, 3.2):SUBDIRS += \
        qndefrecord \
        qndefmessage \
        qnearfieldtagtype1 \
        qnearfieldtagtype2 \
        qnearfieldmanager

#!win32:SUBDIRS += \
#        qbluetoothaddress\
#        qbluetoothuuid\
#        qbluetoothdeviceinfo\
#        qbluetoothdevicediscoveryagent\
#        qbluetoothserviceinfo\
#        qbluetoothservicediscoveryagent\
#        qbluetoothsocket\
#        qrfcommserver \
#        qbluetoothtransferrequest

#enable autotests on symbian
!contains(S60_VERSION, 3.1):!contains(S60_VERSION, 3.2):symbian:SUBDIRS += \
        qbluetoothaddress\
        qbluetoothuuid\
        qbluetoothdeviceinfo\
        qbluetoothdevicediscoveryagent\
        qbluetoothserviceinfo\
        qbluetoothservicediscoveryagent\
        qbluetoothsocket\
        qrfcommserver \
        qbluetoothtransferrequest
