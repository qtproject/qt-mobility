/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QTCONTACTSGLOBAL_H
#define QTCONTACTSGLOBAL_H

#include <qmobilityglobal.h>
#include <QString>
#include <QVariant>

#define QTCONTACTS_VERSION_NAME "com.nokia.qt.mobility.contacts.api.version"
#define QTCONTACTS_IMPLEMENTATION_VERSION_NAME "com.nokia.qt.mobility.contacts.implementation.version"
#define QTCONTACTS_VERSION 1

QTM_BEGIN_NAMESPACE

/*
 * QLatin1Constant
 *
 * The idea of the QLatin1Constant is to provide a POD-esque container
 * for constant strings which are defined in various places
 * (e.g., detail leaf class definition names, field keys, constant field values, etc).
 * We would ideally like these to be stored in the .rodata section to allow
 * sharing / minimise footprint.
 *
 * Given that the declare/define macros are const anyway, we changed the
 * member to a char array from a const char array, in order to squash
 * the compiler warning regarding uninitialised const value without
 * initialiser list in default ctor (POD requires default ctor).
 */

template <int N> struct QLatin1Constant
{
    char chars[N];

    bool operator ==(const QLatin1Constant& other) const {return (chars == other.chars) || (qstrcmp(chars, other.chars) == 0);}
    bool operator !=(const QLatin1Constant& other) const {return !operator==(other);}

    inline const char * latin1() const {return chars;}

    operator QLatin1String() const {return QLatin1String(chars);}
    operator QString() const {return QString::fromLatin1(chars, N-1);}
    operator QVariant() const {return QVariant(operator QString());}
};

/* Hash - this comes from qhash.cpp >.> */
template<int N> uint qHash(const QLatin1Constant<N>& a)
{
    uint h = 0;
    uint g;
    int n = N - 1;
    const register uchar*p = (const uchar*)a.chars;

    while (n--) {
        h = (h << 4) + *p++;
        if ((g = (h & 0xf0000000)) != 0)
            h ^= g >> 23;
        h &= ~g;
    }
    return h;
}

/* Operators for QLatin1Constant */
template<int N, int M> bool operator==(const QLatin1Constant<N>&, const QLatin1Constant<M>&)
{
    // For N != M, this is always false
    // For N == M, the member function gets called
    return false;
}
template<int N, int M> bool operator!=(const QLatin1Constant<N>&, const QLatin1Constant<M>&)
{
    // If N != M, this is always true
    // For N == M, the member function again gets called
    return true;
}

template<int N, int M> bool operator <(const QLatin1Constant<N>& a, const QLatin1Constant<M>& b)
{
    return qstrcmp(a.chars, b.chars) < 0;
}

/* Operators for QLatin1String */
template<int N> bool operator==(const QLatin1Constant<N>& a, const QLatin1String& b)
{
    return (a.chars == b.latin1()) || (qstrcmp(a.chars, b.latin1()) == 0);
}

template<int N> bool operator==(const QLatin1String& b, const QLatin1Constant<N>& a)
{
    return (a.chars == b.latin1()) || (qstrcmp(a.chars, b.latin1()) == 0);
}

template<int N> bool operator!=(const QLatin1Constant<N>& a, const QLatin1String& b)
{
    return (a.chars != b.latin1()) && (qstrcmp(a.chars, b.latin1()) != 0);
}

template<int N> bool operator!=(const QLatin1String& b, const QLatin1Constant<N>& a)
{
    return (a.chars != b.latin1()) && (qstrcmp(a.chars, b.latin1()) != 0);
}

/* Operators for QString */
template<int N> bool operator==(const QLatin1Constant<N>& a, const QString& b)
{
    return b == QLatin1String(a.chars);
}

template<int N> bool operator==(const QString& b, const QLatin1Constant<N>& a)
{
    return b == QLatin1String(a.chars);
}

template<int N> bool operator!=(const QLatin1Constant<N>& a, const QString& b)
{
    return b != QLatin1String(a.chars);
}

template<int N> bool operator!=(const QString& b, const QLatin1Constant<N>& a)
{
    return b != QLatin1String(a.chars);
}

#define Q_DECLARE_LATIN1_CONSTANT(varname, str) static const QLatin1Constant<sizeof(str)> varname
#define Q_DEFINE_LATIN1_CONSTANT(varname, str) const QLatin1Constant<sizeof(str)> varname = {str}

QTM_END_NAMESPACE

// Not needed since this is a typedef, and qglobal already does this for the base type
// Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactLocalId), Q_PRIMITIVE_TYPE);

// Workaround for unannounced SC break
#include "qcontactid.h"

#endif
