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

#ifndef QMLTIMER_H
#define QMLTIMER_H

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
#include <e32base.h>    // For CActive, link against: euser.lib
#include <lbs.h>
#include <lbscommon.h>

QTM_BEGIN_NAMESPACE

#define UPDATE_TIMER 5000000

class CQGeoPositionInfoSourceS60;

class CQMLTimer : public CTimer
{
public:
	
		enum OPERATING_STATE{
			INITIAL,
			HYBRID_RUNNING,
			HYBRID_STOPPED,
			HYBRID_RESTART
		};
	
    // Cancel and destroy
    ~CQMLTimer();

    // Two-phased constructor.
    static CQMLTimer* NewL(QObject *aRequester);

    // Two-phased constructor.
    static CQMLTimer* NewLC(QObject *aRequester);
    
    bool StartTimer(); 
    
    void StopTimer();
    
    bool isTimerStopped(){
    	return mTimerStopped;
    };
    
    void setTrackState(int nTrackState);
    
    int getTrackState(){
    	return mOperatingState;
    }
    
private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    int RunError(int aError);
    
 
private:
		CQMLTimer();
		void ConstructL(QObject *aRequester);
          
private:
	 // Request is a device or a regular
    CQGeoPositionInfoSourceS60 *mRequester;
    
    int	mOperatingState;
    
  	bool mIsTimerOn;
  	
  	bool mTimerStopped;
     
};

QTM_END_NAMESPACE

#endif // QMLTimer_H
