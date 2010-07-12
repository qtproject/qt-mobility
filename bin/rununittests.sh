#!/bin/bash
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the Qt Mobility Components.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the Technology Preview License Agreement accompanying
## this package.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
##
##
##
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################

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

#LD_LIBRARY_PATH=$shell_path/../lib:$LD_LIBRARY_PATH

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
./tst_dummynmeapositioninfosource
./tst_qnmeapositioninfosource_realtime
./tst_qnmeapositioninfosource_realtime_generic
./tst_qnmeapositioninfosource_simulation
./tst_qnmeapositioninfosource_simulation_generic
./tst_qdeclarativeposition

#Publish and Subscribe
./tst_qmallocpool
./tst_qpacket
./tst_qpacketprotocol
./tst_qvaluespace
./tst_qvaluespacepublisher
./tst_qvaluespacesubscriber
./tst_qvaluespacesubscriber_oop
./tst_qsystemreadwritelock
./tst_qsystemreadwritelock_oop
./tst_qcrmlparser

#QSystemInformation
./tst_qsystemdeviceinfo
./tst_qsystemdisplayinfo
./tst_qsysteminfo
./tst_qsystemstorageinfo
./tst_qsystemnetworkinfo
./tst_qsystemscreensaver

#QMedia
./tst_qaudiocapturesource
./tst_qgraphicsvideoitem
./tst_qmediacontent
./tst_qmediaimageviewer
./tst_qmediaobject
./tst_qmediaplayer
./tst_qmediaplaylist
./tst_qmediaplaylistnavigator
./tst_qmediapluginloader
./tst_qmediarecorder
./tst_qmediaresource
./tst_qmediaservice
./tst_qmediaserviceprovider
./tst_qpaintervideosurface
./tst_qradiotuner
./tst_qvideowidget
a
#QContacts
./tst_qcontact
./tst_qcontactactions
./tst_qcontactasync
./tst_qcontactdetail
./tst_qcontactdetaildefinition
./tst_qcontactdetails
./tst_qcontactfilter
./tst_qcontactmanager
./tst_qcontactmanagerfiltering
./tst_qcontactmanagerplugins
./tst_qcontactrelationship
./tst_qvcard21writer
./tst_qvcard30writer
./tst_qversit
./tst_qversitcontactexporter
./tst_qversitcontactimporter
./tst_qversitdocument
./tst_qversitproperty
./tst_qversitreader
./tst_qversitwriter

#QMessaging
./tst_qmessage
./tst_qmessagestore
./tst_qmessagestorekeys

cd $SAVED_PWD

