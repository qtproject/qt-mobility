include(../../staticconfig.pri)

requires(contains(mobility_modules,connectivity))

TEMPLATE = subdirs

SUBDIRS += \
        qndefrecord \
        qndefmessage \
        qnearfieldmanager \
        qnearfieldtagtype1

!win32:SUBDIRS += \
        qbluetoothaddress\
        qbluetoothuuid\
        qbluetoothdeviceinfo\
        qbluetoothdevicediscoveryagent\
        qbluetoothserviceinfo\
        qbluetoothservicediscoveryagent\
        qbluetoothsocket\
        qrfcommserver


