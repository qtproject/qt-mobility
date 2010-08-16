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

#ifndef QMAILGLOBAL_H
#define QMAILGLOBAL_H

#include <qglobal.h>

#ifdef SINGLE_MODULE_QTOPIAMAIL
#define QMF_DECL_EXPORT
#define QMF_DECL_IMPORT
#define QMF_VISIBILITY
#else
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
#define QMF_DECL_EXPORT Q_DECL_EXPORT
#define QMF_DECL_IMPORT Q_DECL_IMPORT
#define QMF_VISIBILITY
#elif defined(QT_VISIBILITY_AVAILABLE)
#define QMF_DECL_EXPORT Q_DECL_EXPORT
#define QMF_DECL_IMPORT Q_DECL_IMPORT
#define QMF_VISIBILITY __attribute__((visibility("default")))
#else
#define QMF_DECL_EXPORT
#define QMF_DECL_IMPORT
#define QMF_VISIBILITY
#endif
#endif

#ifdef QTOPIAMAIL_INTERNAL
#define QTOPIAMAIL_EXPORT QMF_DECL_EXPORT
#else
#define QTOPIAMAIL_EXPORT QMF_DECL_IMPORT
#endif

#ifdef QMFUTIL_INTERNAL
#define QMFUTIL_EXPORT QMF_DECL_EXPORT
#else
#define QMFUTIL_EXPORT QMF_DECL_IMPORT
#endif

#ifdef MESSAGESERVER_INTERNAL
#define MESSAGESERVER_EXPORT QMF_DECL_EXPORT
#else
#define MESSAGESERVER_EXPORT QMF_DECL_IMPORT
#endif

#ifdef PLUGIN_INTERNAL
#define PLUGIN_EXPORT QMF_DECL_EXPORT
#elif defined(PLUGIN_STATIC_LINK)
#define PLUGIN_EXPORT
#else
#define PLUGIN_EXPORT QMF_DECL_IMPORT
#endif

#endif
