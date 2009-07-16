/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTCONTACTSGLOBAL_H
#define QTCONTACTSGLOBAL_H

#include <qglobal.h>
#include <QString>

#ifdef BUILD_QTCONTACTS
#define QTCONTACTS_EXPORT Q_DECL_EXPORT
#else
#define QTCONTACTS_EXPORT Q_DECL_IMPORT
#endif

// XXX
#if defined(EXPORT_PRIVATE)
#define QTCONTACTS_PRIVATE_EXPORT HB_EXPORT
#else
#define QTCONTACTS_PRIVATE_EXPORT
#endif

typedef quint32 QUniqueId; // XXX Put this else where

template <int N> struct ConstantLatinString
{
    const char str[N];
    operator QString() const {return QString(QLatin1String(str));}
};

#define Q_DECLARE_CONSTANT_LATIN_STRING(varname, str) static const ConstantLatinString<sizeof(str)> varname
#define Q_DEFINE_CONSTANT_LATIN_STRING(varname, str) const ConstantLatinString<sizeof(str)> varname = {str}

#endif
