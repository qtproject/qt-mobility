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

#include "xqpublishandsubscribeutils_p.h"
#include "xqsettingsmanager_p.h"
#include "xqsettingsmanager_p.h"
#include "cpublishandsubscribehandler_p.h"
#include "xqpublishandsubscribesecuritypolicy_p.h"

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
