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

#ifndef QMLBACKENDMONITORAO_H_
#define QMLBACKENDMONITORAO_H_

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
#include <lbt.h>
#include <lbtsessiontrigger.h>

#include "qgeoareamonitor.h"

QTM_BEGIN_NAMESPACE

class CBackendMonitorInfo;

enum enTriggerType { EntryTrigger,
                     ExitTrigger,
                     NotifyChangesTrigger,
                     InvalidTrigger = -1
                   };

class QGeoAreaMonitorS60;

class QMLBackendMonitorAO : public CActive
{
public :
    static QMLBackendMonitorAO* NewL(RLbtServer &aLbtServer);

    static void DeleteAO(QGeoAreaMonitorS60* aParent);

    void NotifyFiredEvent();

    void  DoCancel();

    void RunL();


private :
    QMLBackendMonitorAO();

    ~QMLBackendMonitorAO();

    static QMLBackendMonitorAO* NewLC(RLbtServer &aLbt);

    void ConstructL(RLbtServer &aLbtServ);

    bool isValid() {
        return subsessionCreated && (iTriggerMonitorInfo != NULL);
    }
private:
    //Design change
    static QMLBackendMonitorAO* iBackendMonitorAO;      //single instance for access from all the QGeoAreaMonitorS60 objects

    CBackendMonitorInfo*  iTriggerMonitorInfo;  //single instance of the CBackendMonitorInfo object

    static TInt refCount;   //count : number of monitors created by the clients

    TLbtTriggerFireInfo iTriggerInfo;   //info of the fired event

    bool subsessionCreated; //check for the successful creation of the subsession

    RLbt iLbt;  //subsession

};
QTM_END_NAMESPACE

#endif /* QMLBACKENDMONITORAO_H_ */
