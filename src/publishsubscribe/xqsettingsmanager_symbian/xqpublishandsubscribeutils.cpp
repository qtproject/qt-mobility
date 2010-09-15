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

#include "xqpublishandsubscribeutils.h"
#include "xqpublishandsubscribeutils_p.h"

/* 
    \class XQPublishAndSubscribeUtils
    \internal

    \brief The XQPublishAndSubscribeUtils implements Publish and Subscribe related special
    features in SettingsManager. This class is just a Qt-like wrapper to
    Symbian OS' RPropery class.
*/


/* 
    Constructs an XQPublishAndSubscribeUtils object.
    \param settingsManager Reference to XQSettingsManager instance
*/
XQPublishAndSubscribeUtils::XQPublishAndSubscribeUtils(XQSettingsManager& settingsManager, QObject* parent)
    : QObject(parent), d(new XQPublishAndSubscribeUtilsPrivate(settingsManager))
{
}

/* 
    Destructs an XQPublishAndSubscribeUtils object.
*/
XQPublishAndSubscribeUtils::~XQPublishAndSubscribeUtils()
{
}

/* 
    Defines a propery
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \param type Propery type (NOTE: TypeDouble is not supported by Publish and Subscribe.)
    \return True if the property was successfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type)
{
    return d->defineProperty(key, type);
}

/* 
    Defines a propery with security policies
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \param type Propery type (NOTE: TypeDouble is not supported by Publish and Subscribe.)
    \param readPolicy Policy definition for read operations
    \param writePolicy Policy definition for write operations
    \return True if the property was successfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
    const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy)
{
    return d->defineProperty(key, type, readPolicy, writePolicy);
}

/* 
    Deletes a propery
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \return True if the property was successfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::deleteProperty(const XQPublishAndSubscribeSettingsKey& key)
{
    return d->deleteProperty(key);
}
