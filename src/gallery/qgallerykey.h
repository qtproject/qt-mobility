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

#ifndef QGALLERYKEY_H
#define QGALLERYKEY_H

#include <qgalleryfilter.h>

#include <QtCore/qstring.h>

class Q_GALLERY_EXPORT QGalleryKey
{
public:
    template <int N>
    explicit QGalleryKey(const char (&id)[N]) : m_id(id), m_length(N - 1) {}

    QString toString() const;

    inline operator QString() const { return toString(); }

    QGalleryMetaDataFilter matches(const QVariant &value, Qt::MatchFlags flags = 0);
    QGalleryMetaDataRangeFilter isInRange(const QVariant &minimum, const QVariant &maximum) const;

    QGalleryMetaDataRangeFilter operator <(const QVariant &value) const;
    QGalleryMetaDataRangeFilter operator <=(const QVariant &value) const;

    QGalleryMetaDataFilter operator ==(const QVariant &value) const;

    QGalleryMetaDataRangeFilter operator >=(const QVariant &value) const;
    QGalleryMetaDataRangeFilter operator >(const QVariant &value) const;

    QString ascending() const;
    QString descending() const;

private:
    const char * const m_id;
    const int m_length;
};



#endif
