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

#ifndef QGALLERYTYPE_H
#define QGALLERYTYPE_H

#include <qmobilityglobal.h>

#include <QtCore/qstring.h>

QTM_BEGIN_NAMESPACE

struct Q_GALLERY_EXPORT QGalleryType
{
    QString name() const { return QString::fromLatin1(m_name, m_length); }

    inline operator QString() const { return QString::fromLatin1(m_name, m_length); }

    const char * const m_name;
    const int m_length;
};

#define Q_DEFINE_GALLERY_TYPE(scope, name) const QGalleryType scope::name = {#name, sizeof(#name) - 1};

QTM_END_NAMESPACE

#endif
