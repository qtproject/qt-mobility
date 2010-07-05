/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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


#include <qdeclarativeextensionplugin.h>

#include <qdeclarativeengine.h>
#include <qdeclarative.h>
#include <qcontactmanager.h>
#include <qcontactdetails.h>
#include "imageprovider.h"


QTM_USE_NAMESPACE

// This is run in a low priority thread.
QImage ContactThumbnailImageProvider::request(const QString &id, QSize *size, const QSize &req_size)
{
    if (m_thumbnails.contains(id)) {
        if (size)
            *size = req_size;
        return m_thumbnails.value(id).scaled(req_size);
    }

    /* url format:
        image://thumbnail/{manager.contactid}
     */

    QString managerName = id.split('.').first();
    QString localId = id.split('.').last();

    QContactManager* manager = 0;
    if (m_managers.contains(managerName)) {
        manager = m_managers.value(managerName);
    } else {
        manager = new QContactManager(managerName);
        m_managers.insert(managerName, manager);
    }

    QContact c = manager->contact(localId.toInt());

    QImage image(
            req_size.width() > 0 ? req_size.width() : 100,
            req_size.height() > 0 ? req_size.height() : 50,
            QImage::Format_RGB32);

    QContactThumbnail t = c.detail<QContactThumbnail>();
    if (!t.thumbnail().isNull()) {
        image = t.thumbnail().scaled(image.size());
    } else {
        QContactAvatar a = c.detail<QContactAvatar>();
        QString imageUrl = a.imageUrl().isEmpty()? QString::fromLatin1("contents/default.svg") : a.imageUrl().toString();
        image.load(imageUrl);
    }

    if (size)
        *size = image.size();

    m_thumbnails.insert(id, image);

    return image;
}

ContactThumbnailImageProvider::~ContactThumbnailImageProvider()
{
    foreach(const QString& name, m_managers.keys()) {
        delete m_managers.value(name);
    }

    m_managers.clear();
}


