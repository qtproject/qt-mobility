/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPOSITIONINFOSOURCESYMBIAN_H
#define QGEOPOSITIONINFOSOURCESYMBIAN_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbs.h>
#include <qstring.h>
#include "qgeopositioninfosource.h"
//#include "qmlbackendao_s60_p.h"
//#include "notificationcallback_s60_p.h"

#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include "QGeoPositionInfo.h"
#include "qgeopositioninfosource.h"
#include <qgeopositioninfosource.h>

#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

//#include "clbstrackingtequestors_60_p.h"
#include "symbian_lbsfacade_p.h"
#include "symbian_lbstracker_p.h"
#include "symbian_lbsonetime_p.h"

//#define MAX_SIZE 25

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class CLbsTrackingRequestorBase;
class CPsyContainer;
/**
 *  CQGeoPositionInfoSourceSymbian
 *
 */


class CQGeoPositionInfoSourceSymbian : public QGeoPositionInfoSource,
        public MLbsTrackingCallback,
        public MLbsSingleShotCallback

{
public:
    // Constructors and destructor
    /**
     * Destructor.
     */
    ~CQGeoPositionInfoSourceSymbian();

    /**
     * Two-phased constructor.
     */
    static CQGeoPositionInfoSourceSymbian* NewL(QObject* aParent);

    /**
     * returns the last known position
     */
    QGeoPositionInfo lastKnownPosition(bool aFromSatellitePositioningMethodsOnly = false) const;

    /**
     * returns the minimum update interval
     */
    int minimumUpdateInterval() const;

    /**
     * Sets the preferred PositioningMethod, if available
     * otherwise sets the default poistioning mmethod
     */
    void setPreferredPositioningMethods(PositioningMethods aMethods);

    /**
     * Sets the interval for the regular position notifications
     */
    void setUpdateInterval(int aMilliSec);

    /**
     * Returns the supported Positioning Methods
     */
    PositioningMethods supportedPositioningMethods() const {
        return mSupportedMethods;
    }





public slots :
    // for request update
    void requestUpdate(int timeout = 5000);

    // starts the regular updates
    virtual void startUpdates();

    // stops the regular updates
    virtual void stopUpdates();


private:

    /**
     * Constructor for performing 1st stage construction
     */
    CQGeoPositionInfoSourceSymbian(QObject* aParent = 0);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    void TPositionInfo2QGeoPositionInfo(HPositionGenericInfo *mPosInfo,
                                        QGeoPositionInfo& posUpdate);

    void TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aBase);
    void TrackingRequestTimedOut(CLbsPositionTrackerBase *aBase);

    void SSLocation(QGeoPositionInfo *aPosition, CLbsSingleShotRequestor *aRequestor);
    void SSRequestTimedOut(CLbsSingleShotRequestor *aRequestor);

private:

    CLbsPositionTrackerBase *iTrackingRequestor;
    CLbsSingleShotRequestor *iSSRequestor;

    QGeoPositionInfo mLastPosInfo;

    CPsyContainer *iPsyContainer;

    PositioningMethods mPreferredMethods;

    PositioningMethods mSupportedMethods;

    TBool iTrackingInProgress;

    TBool iTimeOutCanBeSent;

};




QTM_END_NAMESPACE

QT_END_HEADER

#endif // CQGEOPOSITIONINFOSOURCES60_H
