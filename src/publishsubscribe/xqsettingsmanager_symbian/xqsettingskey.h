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

#ifndef XQSETTINGSITEM_H
#define XQSETTINGSITEM_H

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
#include "settingsmanager_global.h"

// CLASS DECLARATION
class XQSETTINGSMANAGER_EXPORT XQSettingsKey
{
public:
    enum Target
    {
        TargetCentralRepository = 0,
        TargetPublishAndSubscribe
    };

    XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key);
    ~XQSettingsKey();

    XQSettingsKey::Target target() const;
    long int uid() const;
    unsigned long int key() const;

protected:
    XQSettingsKey::Target m_target;
    long int m_uid;
    unsigned long int m_key;
};

class XQSETTINGSMANAGER_EXPORT XQCentralRepositorySettingsKey : public XQSettingsKey
{
public:
    XQCentralRepositorySettingsKey(long int repositoryUid, unsigned long int key);
    ~XQCentralRepositorySettingsKey();
};

class XQSETTINGSMANAGER_EXPORT XQPublishAndSubscribeSettingsKey : public XQSettingsKey
{
public:
    XQPublishAndSubscribeSettingsKey(long int categoryUid, unsigned long int key);
    ~XQPublishAndSubscribeSettingsKey();
};

#endif //XQSETTINGSITEM_H
