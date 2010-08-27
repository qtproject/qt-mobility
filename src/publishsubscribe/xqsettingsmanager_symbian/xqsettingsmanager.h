/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

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
#include <QObject>
#include <QVariant>
#include "xqsettingskey.h"
#include "settingsmanager_global.h"

// FORWARD DECLARATIONS
class XQSettingsManagerPrivate;

// CLASS DECLARATION
class XQSETTINGSMANAGER_EXPORT XQSettingsManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_ENUMS(Type)

public:

    enum Type
    {
        TypeVariant = 0,
        TypeInt,
        TypeDouble,
        TypeString,
        TypeByteArray
    };

    enum Error 
    {
        NoError = 0,
        OutOfMemoryError,
        NotFoundError,
        AlreadyExistsError,
        PermissionDeniedError,
        BadTypeError,
        NotSupportedError,
        UnknownError = -1
    };

    XQSettingsManager(QObject* parent = 0);
    ~XQSettingsManager();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool stopMonitoring(const XQSettingsKey& key);
    
    XQSettingsManager::Error error() const;

Q_SIGNALS:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);
    void itemDeleted(const XQSettingsKey& key);

private:
    friend class XQSettingsManagerPrivate;
    friend class XQCentralRepositoryUtilsPrivate;
    friend class XQPublishAndSubscribeUtilsPrivate;
    XQSettingsManagerPrivate* d;
};

#endif // XQSETTINGSMANAGER_H

// End of file
