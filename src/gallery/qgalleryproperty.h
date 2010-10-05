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

#ifndef QGALLERYPROPERTY_H
#define QGALLERYPROPERTY_H

#include <qgalleryfilter.h>

#include <QtCore/qstring.h>

QTM_BEGIN_NAMESPACE

class Q_GALLERY_EXPORT QGalleryProperty
{
public:
    enum Attribute
    {
        CanRead  = 0x01,
        CanWrite = 0x02,
        CanSort = 0x04,
        CanFilter = 0x08
    };

    Q_DECLARE_FLAGS(Attributes, Attribute)

#ifndef Q_QDOC
    template <int N>
    explicit QGalleryProperty(const char (&name)[N]) : m_name(name), m_length(N - 1) {}
#else
    explicit QGalleryProperty(const char *name);
#endif

    QString name() const;

    inline operator QString() const { return name(); }

    QGalleryMetaDataFilter operator <(const QVariant &value) const;
    QGalleryMetaDataFilter operator <=(const QVariant &value) const;

    QGalleryMetaDataFilter operator ==(const QVariant &value) const;

    QGalleryMetaDataFilter operator >=(const QVariant &value) const;
    QGalleryMetaDataFilter operator >(const QVariant &value) const;

    QGalleryMetaDataFilter contains(const QString &string) const;
    QGalleryMetaDataFilter startsWith(const QString &string) const;
    QGalleryMetaDataFilter endsWith(const QString &string) const;
    QGalleryMetaDataFilter wildcard(const QString &string) const;
    QGalleryMetaDataFilter regExp(const QString &regExp) const;
    QGalleryMetaDataFilter regExp(const QRegExp &regExp) const;

    QString ascending() const;
    QString descending() const;

private:
    const char * const m_name;
    const int m_length;
};

QTM_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(QTM_PREPEND_NAMESPACE(QGalleryProperty::Attributes));

#endif
