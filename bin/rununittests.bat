:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: Contact: Nokia Corporation (qt-info@nokia.com)
::
:: This file is part of the Qt Mobility Components.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: No Commercial Usage
:: This file contains pre-release code and may not be distributed.
:: You may use this file in accordance with the terms and conditions
:: contained in the Technology Preview License Agreement accompanying
:: this package.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Nokia gives you certain additional
:: rights.  These rights are described in the Nokia Qt LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: If you have questions regarding the use of this file, please contact
:: Nokia at qt-info@nokia.com.
::
::
::
::
::
::
::
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\lib;%PATH%

cd %BATCH_PATH%..\build\tests\bin

::ServiceFramework
tst_servicemetadata.exe
tst_servicedatabase.exe
tst_databasemanager.exe
tst_qservicemanager.exe
tst_qservicefilter.exe
tst_qserviceinterfacedescriptor.exe
tst_qabstractsecuritysession.exe
tst_qservicecontext.exe
tst_qmetaobjectbuilder.exe
tst_icheck.exe
tst_qservicemanager_ipc.exe
tst_qremoteserviceclassregister.exe

::BearerManagement
tst_qnetworkconfiguration.exe
tst_qnetworkconfigurationmanager.exe
tst_qnetworksession.exe

::Location
tst_qgeocoordinate.exe
tst_qgeopositioninfo.exe
tst_qgeosatelliteinfo.exe
tst_qgeosatelliteinfosource.exe
tst_qgeopositioninfosource.exe
tst_qgeoareamonitor.exe
tst_qlocationutils.exe
tst_dummynmeapositioninfosource.exe
tst_qnmeapositioninfosource_realtime.exe
tst_qnmeapositioninfosource_realtime_generic.exe
tst_qnmeapositioninfosource_simulation.exe
tst_qnmeapositioninfosource_simulation_generic.exe

::Publish and Subscribe
tst_qvaluespace.exe
tst_qvaluespaceprovider.exe
tst_qvaluespacesubscriber.exe
tst_qvaluespacesubscriber_oop.exe
tst_qsystemreadwritelock.exe
tst_qsystemreadwritelock_oop.exe
tst_qcrmlparser.exe

::QSystemInformation
tst_qsystemdeviceinfo.exe
tst_qsystemdisplayinfo.exe
tst_qsysteminfo.exe
tst_qsystemnetworkinfo.exe
tst_qsystemscreensaver.exe
tst_qsystemstorageinfo.exe

::QMedia
tst_qaudiocapturesource.exe
tst_qgraphicsvideoitem.exe
tst_qmediacontent.exe
tst_qmediaimageviewer.exe
tst_qmediaobject.exe
tst_qmediaplayer.exe
tst_qmediaplaylist.exe
tst_qmediaplaylistnavigator.exe
tst_qmediapluginloader.exe
tst_qmediarecorder.exe
tst_qmediaresource.exe
tst_qmediaservice.exe
tst_qmediaserviceprovider.exe
tst_qpaintervideosurface.exe
tst_qradiotuner.exe
tst_qvideowidget.exe

::QContacts
tst_qcontact.exe
tst_qcontactactions.exe
tst_qcontactasync.exe
tst_qcontactdetail.exe
tst_qcontactdetaildefinition.exe
tst_qcontactdetails.exe
tst_qcontactfilter.exe
tst_qcontactmanager.exe
tst_qcontactmanagerfiltering.exe
tst_qcontactmanagerplugins.exe
tst_qcontactrelationship.exe
tst_qvcard21writer.exe
tst_qvcard30writer.exe
tst_qversitcontactexporter.exe
tst_qversitcontactimporter.exe
tst_qversitdocument.exe
tst_qversitproperty.exe
tst_qversitreader.exe
tst_qversitwriter.exe
tst_versitutils.exe

::QMessaging
tst_qmessage.exe
tst_qmessagestore.exe
tst_qmessagestorekeys.exe

cd %SAVED_PWD%

