/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef XQSETTINGSMANAGER_P_H
#define XQSETTINGSMANAGER_P_H

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
#include "xqsettingsmanager.h"
#include <QHash>

#include "xqsettingskey.h"
#include "settingshandlerbase.h"

// FORWARD DECLARATIONS
class QVariant;
class CCentralRepositoryHandler;
class CPublishAndSubscribeHandler;

// CLASS DECLARATION
class XQSettingsManagerPrivate
    : public CBase, public MSettingsHandlerObserver
{
public:
    XQSettingsManagerPrivate(XQSettingsManager* settingsmanager);
    ~XQSettingsManagerPrivate();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type);
    bool stopMonitoring(const XQSettingsKey& key);

    XQSettingsManager::Error error() const;

    CCentralRepositoryHandler* cenRepHandlerInstance(long int repositoryUid);
    CPublishAndSubscribeHandler* pubSubHandlerInstance(long int categoryUid);

private:
    SettingsHandlerBase* handlerInstance(const XQSettingsKey& key);

protected:  //MSettingsHandlerObserver
    void valueChanged(const XQSettingsKey& key, XQSettingsManager::Type type);
    void itemDeleted(const XQSettingsKey& key);

private:
    friend class XQCentralRepositoryUtilsPrivate;
    friend class XQPublishAndSubscribeUtilsPrivate;
    XQSettingsManager* q;
    mutable int iError;
    QHash<long int, CCentralRepositoryHandler*> m_centralRepositoryHandlers;
    QHash<long int, CPublishAndSubscribeHandler*> m_publishAndSubscribeHandlers;
};

#endif /*XQSETTINGSMANAGER_P_H*/

// End of file
