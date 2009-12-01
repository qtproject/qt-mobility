/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PUBLISHANDSUBSCRIBEHANDLER_H
#define PUBLISHANDSUBSCRIBEHANDLER_H

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

#include <e32base.h>

#include "settingshandlerbase.h"
#include "pubsubmonitor.h"
#include "xqpublishandsubscribesecuritypolicy.h"

class XQSettingsKey;

class CPublishAndSubscribeHandler : public CBase, public SettingsHandlerBase
{
public:
    static CPublishAndSubscribeHandler* NewL(TUid aUid);
    ~CPublishAndSubscribeHandler();

    TInt defineProperty(unsigned long key, XQSettingsManager::Type type);
    TInt defineProperty(unsigned long key, XQSettingsManager::Type type,
        const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy);
    TInt deleteProperty(unsigned long key);

private:
    CPublishAndSubscribeHandler(TUid aUid);
    void ConstructL();

protected:
    void setObserver(MSettingsHandlerObserver* observer);
    bool handleStartMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type, MSettingsHandlerObserver& observer, TInt& error);
    bool handleStopMonitoring(const XQSettingsKey& key, TInt& error);

    TInt getValue(unsigned long key, TInt& value);
    TInt getValue(unsigned long key, TReal& value);
    void getValueL(unsigned long key, RBuf8& value);
    void getValueL(unsigned long key, RBuf16& value);

    TInt setValue(unsigned long key, const TInt& value);
    TInt setValue(unsigned long key, const TReal& value);
    TInt setValue(unsigned long key, const TDesC8& value);
    TInt setValue(unsigned long key, const TDesC16& value);

private:
    TSecurityPolicy symbianPolicy(const XQPublishAndSubscribeSecurityPolicy& policy);
    TCapability symbianCapability(const XQPublishAndSubscribeSecurityPolicy::Capability& capability);
private:
    MSettingsHandlerObserver* m_observer;
    TUid m_uid;
    QHash<unsigned long int, CPubSubMonitor*> m_monitors;
};

#endif //PUBLISHANDSUBSCRIBEHANDLER_H
