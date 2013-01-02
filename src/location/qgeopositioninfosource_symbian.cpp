/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeopositioninfosource_symbian_p.h"
//#include "clbstrackingtequestors_60_p.h"
//#include "clbspsycontainer_s60_p.h"
#include "symbian_lbsfacade_p.h"
#include "symbian_lbstracker_p.h"
#include "symbian_lbsonetime_p.h"

#include "qgeopositioninfosource.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

CQGeoPositionInfoSourceSymbian::~CQGeoPositionInfoSourceSymbian()
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::~CQGeoPositionInfoSourceSymbian ";
    delete iTrackingRequestor;
    delete iSSRequestor;
}

CQGeoPositionInfoSourceSymbian* CQGeoPositionInfoSourceSymbian::NewL(QObject* aParent)
{
    CQGeoPositionInfoSourceSymbian *posInfo = new(ELeave) CQGeoPositionInfoSourceSymbian();
    posInfo->ConstructL();
    return posInfo;
}

CQGeoPositionInfoSourceSymbian::CQGeoPositionInfoSourceSymbian(QObject* aParent)
        : QGeoPositionInfoSource(aParent), iTrackingRequestor(NULL), iSSRequestor(NULL),
        mSupportedMethods(0xffffffff), mPreferredMethods(0xffffffff),
        iTrackingInProgress(EFalse), iTimeOutCanBeSent(ETrue)
{

}

void CQGeoPositionInfoSourceSymbian::ConstructL()
{
    iPsyContainer = CPsyContainer::NewL();
    iTrackingRequestor = CLbsAllPosProxy::NewL(iPsyContainer, this);
}

QGeoPositionInfo CQGeoPositionInfoSourceSymbian::lastKnownPosition(bool aFromSatellitePositioningMethodsOnly) const
{
    return iPsyContainer->lastKnownPosition(aFromSatellitePositioningMethodsOnly);
}

int CQGeoPositionInfoSourceSymbian::minimumUpdateInterval() const
{
    return iTrackingRequestor->GetMinimumUpdateInterval();
}

void CQGeoPositionInfoSourceSymbian::setPreferredPositioningMethods(PositioningMethods aNewMethods)

{
    PositioningMethods curMethods = preferredPositioningMethods();

    if (curMethods == aNewMethods)
        return;

    QGeoPositionInfoSource::setPreferredPositioningMethods(aNewMethods);

    if (iTrackingInProgress) {
        if (QGeoPositionInfoSource::SatellitePositioningMethods == curMethods) {
            if (QGeoPositionInfoSource::NonSatellitePositioningMethods == aNewMethods) {
                delete iTrackingRequestor;
                iTrackingRequestor = CLbsSatellitePositionTracker::NewL(iPsyContainer, this);
            } else { //All Positioning methods
                CLbsSatellitePositionTracker *satTracker = static_cast<CLbsSatellitePositionTracker*>(iTrackingRequestor);
                iTrackingRequestor = CLbsAllPosProxy::NewL(iPsyContainer, this, satTracker, NULL);
            }
            iTrackingRequestor->SetUpdateInterval(updateInterval());
            iTrackingRequestor->StartUpdates();
        } else if (QGeoPositionInfoSource::NonSatellitePositioningMethods == curMethods) {
            if (QGeoPositionInfoSource::SatellitePositioningMethods == aNewMethods) {
                delete iTrackingRequestor;
                iTrackingRequestor = CLbsNonSatellitePositionTracker::NewL(iPsyContainer, this);
            } else { //All positioning methods
                CLbsNonSatellitePositionTracker *NWTracker = static_cast<CLbsNonSatellitePositionTracker*>(iTrackingRequestor);
                iTrackingRequestor = CLbsAllPosProxy::NewL(iPsyContainer, this, NULL, NWTracker);
            }
            iTrackingRequestor->SetUpdateInterval(updateInterval());
            iTrackingRequestor->StartUpdates();
        } else { /*GeoPositionInfoSource::AllPositioningMethods*/
            CLbsAllPosProxy *tracker = static_cast<CLbsAllPosProxy*>(iTrackingRequestor);

            if (QGeoPositionInfoSource::SatellitePositioningMethods == aNewMethods) {
                iTrackingRequestor = tracker->ReleaseSatelliteTracker();
                iTrackingRequestor->SetObserver(this);
            } else { //NW Positioning method
                iTrackingRequestor = tracker->ReleaseNWTracker();
                iTrackingRequestor->SetObserver(this);
            }
            delete tracker;
            //No need to call set update interval or start updates
        }
    }
    //Single shot requests will use the new preferrences from the next requestUpdate
}

void CQGeoPositionInfoSourceSymbian::setUpdateInterval(int aMilliSec)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::setUpdateInterval " << aMilliSec;
    QGeoPositionInfoSource::setUpdateInterval(aMilliSec);
    iTrackingRequestor->SetUpdateInterval(aMilliSec);
}

void CQGeoPositionInfoSourceSymbian::requestUpdate(int timeout)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::requestUpdate";
    delete iSSRequestor;

    CLbsSingleShotRequestor::SSType type;

    if (QGeoPositionInfoSource::AllPositioningMethods == preferredPositioningMethods())
        type = CLbsSingleShotRequestor::SS_AllPsys;
    else if (QGeoPositionInfoSource::SatellitePositioningMethods == preferredPositioningMethods())
        type = CLbsSingleShotRequestor::SS_GPSOnly;
    else
        type = CLbsSingleShotRequestor::SS_NWOnly;

    TRAPD(err, iSSRequestor = CLbsSingleShotRequestor::NewL(iPsyContainer, this, type));

    if (err != KErrNone) {
        return;
    }
    iSSRequestor->RequestLocation(timeout);
    iTimeOutCanBeSent = ETrue;
}

void CQGeoPositionInfoSourceSymbian::startUpdates()
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::startUpdates";

    iTrackingRequestor->StartUpdates();
    iTrackingInProgress = ETrue;
    iTimeOutCanBeSent = ETrue;
}

void CQGeoPositionInfoSourceSymbian::stopUpdates()
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::stopUpdates";
    iTrackingInProgress = EFalse;
    iTrackingRequestor->StopUpdates();
}
void CQGeoPositionInfoSourceSymbian::TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aBase)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::TrackingLocation ";
    //emit posUpdate
    iTimeOutCanBeSent = ETrue;
    emit positionUpdated(*aPosition);
}
void CQGeoPositionInfoSourceSymbian::TrackingRequestTimedOut(CLbsPositionTrackerBase *aBase)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::TrackingRequestTimedOut ";
    if (iTimeOutCanBeSent) {
        emit updateTimeout();
        iTimeOutCanBeSent = EFalse;
    }
}
void CQGeoPositionInfoSourceSymbian::SSLocation(QGeoPositionInfo *aPosition, CLbsSingleShotRequestor *aRequestor)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::SSLocation ";
    emit positionUpdated(*aPosition);
    iTimeOutCanBeSent = ETrue;
}
void CQGeoPositionInfoSourceSymbian::SSRequestTimedOut(CLbsSingleShotRequestor *aRequestor)
{
    qDebug() << "CQGeoPositionInfoSourceSymbian::SSRequestTimedOut ";

    if (iTimeOutCanBeSent) {
        emit updateTimeout();
        iTimeOutCanBeSent = EFalse;
    }
}
QTM_END_NAMESPACE
