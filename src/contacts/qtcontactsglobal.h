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

#include <qmobilityglobal.h>
#include <QString>

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
 * Does it work as hoped?
 */

template <int N> struct QLatin1Constant
{
    //const char str[N]; // causes compiler warning due to uninitialized const value
    char str[N];

    bool operator ==(const QLatin1Constant& other) {return (str == other.str) || (qstrcmp(str, other.str) == 0);}
    operator QLatin1String() const {return QLatin1String(str);}
    operator QString() const {return QString::fromLatin1(str, N-1);}
};

/* Operators for QLatin1String */
template<int N> bool operator==(const QLatin1Constant<N>& a, const QLatin1String& b)
{
    return (a.str == b.latin1()) || (qstrcmp(a.str, b.latin1()) == 0);
}

template<int N> bool operator==(const QLatin1String& b, const QLatin1Constant<N>& a)
{
    return (a.str == b.latin1()) || (qstrcmp(a.str, b.latin1()) == 0);
}

template<int N> bool operator!=(const QLatin1Constant<N>& a, const QLatin1String& b)
{
    return (a.str != b.latin1()) && (qstrcmp(a.str, b.latin1()) != 0);
}

template<int N> bool operator!=(const QLatin1String& b, const QLatin1Constant<N>& a)
{
    return (a.str != b.latin1()) && (qstrcmp(a.str, b.latin1()) != 0);
}

/* Operators for QString */
template<int N> bool operator==(const QLatin1Constant<N>& a, const QString& b)
{
    return b == QLatin1String(a.str);
}

template<int N> bool operator==(const QString& b, const QLatin1Constant<N>& a)
{
    return b == QLatin1String(a.str);
}

template<int N> bool operator!=(const QLatin1Constant<N>& a, const QString& b)
{
    return b != QLatin1String(a.str);
}

template<int N> bool operator!=(const QString& b, const QLatin1Constant<N>& a)
{
    return b != QLatin1String(a.str);
}

#define Q_DECLARE_LATIN1_CONSTANT(varname, str) static const QLatin1Constant<sizeof(str)> varname
#define Q_DEFINE_LATIN1_CONSTANT(varname, str) const QLatin1Constant<sizeof(str)> varname = {str}

/* XXX - deprecation stuff */
#define Q_DECLARE_LATIN1_LITERAL(v, s) Q_DECLARE_LATIN1_CONSTANT(v, s)
#define Q_DEFINE_LATIN1_LITERAL(v, s) Q_DEFINE_LATIN1_CONSTANT(v, s)
template <int N> struct Latin1Literal : public QLatin1Constant<N>
{ };

QTM_END_NAMESPACE

// Not needed since this is a typedef, and qglobal already does this for the base type
// Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactLocalId), Q_PRIMITIVE_TYPE);

// Workaround for unannounced SC break
#include "qcontactid.h"

#endif
