/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTELEPHONYCALLLIST_SYMBIAN_P_H
#define QTELEPHONYCALLLIST_SYMBIAN_P_H

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

#include <qmobilityglobal.h>
#include <QList>
#include <QtCore/qshareddata.h>

#include "qtelephonycalllist.h"
#include "qtelephony.h"
#include <e32base.h>
#include <etelmm.h>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QTelephonyCallInfoPrivate;
class QTelephonyCallList;

class CLineStatusObserver : public CActive
    {
public:
    CLineStatusObserver(QTelephonyCallListPrivate *aList, TInt aLineIndex);
    virtual ~CLineStatusObserver();

public:
    void startMonitoring();
    RMobileCall::TMobileCallStatus m_callStatus;

private:
    void issueNotifyStatusChange();
    QTelephony::CallStatus mapStatus(TInt aStatus); 
    QTelephony::CallType mapType(TInt aType);
    QString toQString(const TDesC& aText);
    void updateCallList();

    // from CActive
    void RunL();
    void DoCancel();
    
    QTelephonyCallListPrivate* m_list;
    RTelServer m_etel;
    RPhone m_phone;
    RMobileLine m_line;
    TInt m_lineIndex;
    bool m_setupPhase;
};

class QTelephonyCallListPrivate
{
    friend class QTelephonyCallList;
    friend class CLineStatusObserver;

public:
    QTelephonyCallListPrivate(QTelephonyCallList *parent = 0);
    virtual ~QTelephonyCallListPrivate();
    QList<QTelephonyCallInfo> activeCalls(const QTelephony::CallType& calltype) const;
    int activeCallCount() const;
    // Public for testing purposes
    QList<QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> > callInfoList;
    QTelephonyCallList* p;
    void emitActiveCallStatusChanged(QTelephonyCallInfoPrivate& call);
    void emitActiveCallRemoved(QTelephonyCallInfoPrivate& call);
    void emitActiveCallAdded(QTelephonyCallInfoPrivate& call);
    
private:
    void checkMobileLines();
    TInt findCallInfo(QString aCallName);
    RTelServer m_etel;
    RPhone m_phone;
    QList<CLineStatusObserver *> m_observers;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif // QTELEPHONYCALLLIST_SYMBIAN_P_H
