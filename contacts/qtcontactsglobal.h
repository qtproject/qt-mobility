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

#ifndef QTCONTACTSGLOBAL_H
#define QTCONTACTSGLOBAL_H

#if defined(QTM_BUILD_UNITTESTS)
# include <qconfig.h>
# if !defined(QT_BUILD_INTERNAL)
#   define QT_BUILD_INTERNAL
# endif
#endif

#include <qglobal.h>
#include <QString>

#ifdef BUILD_QTCONTACTS
#define QTCONTACTS_EXPORT Q_DECL_EXPORT
#else
#define QTCONTACTS_EXPORT Q_DECL_IMPORT
#endif

typedef quint32 QContactLocalId; // XXX Put this else where

template <int N> struct Latin1Literal
{
    const char str[N];

    operator QLatin1String() const {return QLatin1String(str);}
    operator QString() const {return QString::fromLatin1(str, N-1);}
};

template<int N> bool operator==(const Latin1Literal<N>& a, const QLatin1String& b)
{
    return QLatin1String(a.str) == b;
}

template<int N> bool operator==(const Latin1Literal<N>& a, const QString& b)
{
    return b == QLatin1String(a.str);
}

template<int N> bool operator==(const QLatin1String& b, const Latin1Literal<N>& a)
{
    return QLatin1String(a.str) == b;
}

template<int N> bool operator==(const QString& b, const Latin1Literal<N>& a)
{
    return b == QLatin1String(a.str);
}

template<int N> bool operator!=(const Latin1Literal<N>& a, const QLatin1String& b)
{
    return QLatin1String(a.str) != b;
}

template<int N> bool operator!=(const Latin1Literal<N>& a, const QString& b)
{
    return b != QLatin1String(a.str);
}

template<int N> bool operator!=(const QLatin1String& b, const Latin1Literal<N>& a)
{
    return QLatin1String(a.str) != b;
}

template<int N> bool operator!=(const QString& b, const Latin1Literal<N>& a)
{
    return b != QLatin1String(a.str);
}

#define Q_DECLARE_LATIN1_LITERAL(varname, str) static const Latin1Literal<sizeof(str)> varname
#define Q_DEFINE_LATIN1_LITERAL(varname, str) const Latin1Literal<sizeof(str)> varname = {str}

#endif
