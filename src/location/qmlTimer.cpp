/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeopositioninfosource_s60_p.h"
#include "qmlbackendao_s60_p.h"
#include <QDebug>
#include "qmltimer.h"

QTM_BEGIN_NAMESPACE

CQMLTimer::~CQMLTimer()
{
}

CQMLTimer::CQMLTimer():
CTimer(CActive::EPriorityStandard)
{
}

CQMLTimer* CQMLTimer::NewL(QObject *aRequester)
{
		qDebug() << "CQMLTimer::NewL\n" ;
	CQMLTimer* self = CQMLTimer::NewLC(aRequester);
		CleanupStack::Pop(); // self;
    return self;
}

CQMLTimer* CQMLTimer::NewLC(QObject *aRequester)
{
	qDebug() << "CQMLTimer::NewLC\n" ;
	 CQMLTimer* self = new(ELeave) CQMLTimer();
    CleanupStack::PushL(self);
    self->ConstructL(aRequester);
    return self;
}

void CQMLTimer::ConstructL(QObject *aRequester)
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	mRequester = static_cast<CQGeoPositionInfoSourceS60*>(aRequester);
}
   
void CQMLTimer::RunL()
{
	qDebug() << "CQMLTimer::RunL \n";

	if (!mRequester->isUpdates()){
		qDebug() << "Backup updates not stopped \n";
		CTimer::After(30000000);
	}
}
    
void CQMLTimer::DoCancel()
{
	qDebug() << "CQMLTimer::DoCancel \n";
	CTimer::Cancel();
}

bool CQMLTimer::StartTimer()
{
	qDebug() << "CQMLTimer::StartTimer \n";
	CTimer::After(30000000);
	
}

QTM_END_NAMESPACE

