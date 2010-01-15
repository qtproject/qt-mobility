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

#include "xqpublishandsubscribeutils_p.h"
#include "xqsettingsmanager.h"
#include "xqsettingsmanager_p.h"
#include "cpublishandsubscribehandler.h"
#include "xqpublishandsubscribesecuritypolicy.h"

XQPublishAndSubscribeUtilsPrivate::XQPublishAndSubscribeUtilsPrivate(XQSettingsManager& settingsManager)
    : m_settingsManagerPrivate(*settingsManager.d)
{
}

XQPublishAndSubscribeUtilsPrivate::~XQPublishAndSubscribeUtilsPrivate()
{
}

bool XQPublishAndSubscribeUtilsPrivate::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->defineProperty(key.key(), type);
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;
}


bool XQPublishAndSubscribeUtilsPrivate::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
    const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->defineProperty(key.key(), type, readPolicy, writePolicy);
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;    
}

bool XQPublishAndSubscribeUtilsPrivate::deleteProperty(const XQPublishAndSubscribeSettingsKey& key)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->deleteProperty(key.key());
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;
}
