include(../../staticconfig.pri)

requires(contains(mobility_modules,connectivity))

TEMPLATE = subdirs

SUBDIRS += \
        qndefrecord \
        qndefmessage \
        qnearfieldtagtype1
#        qnearfieldmanager \

!win32:SUBDIRS += \
        qbluetoothaddress\
        qbluetoothuuid\
        qbluetoothdeviceinfo\
        qbluetoothdevicediscoveryagent\
        qbluetoothserviceinfo\
        qbluetoothservicediscoveryagent\
        qbluetoothsocket\
        qrfcommserver


