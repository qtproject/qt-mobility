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

#ifndef QVERSITORGANIZERHANDLER_H
#define QVERSITORGANIZERHANDLER_H

#include "qversitorganizerimporter.h"
#include "qversitorganizerexporter.h"

QTM_BEGIN_NAMESPACE

// qdoc seems to not find QVersitOrganizerHandler if it is declared first, hence this forward
// declaration
class QVersitOrganizerHandler;

class Q_VERSIT_ORGANIZER_EXPORT QVersitOrganizerHandlerFactory
{
public:
    virtual ~QVersitOrganizerHandlerFactory() {}
    virtual QSet<QString> profiles() const { return QSet<QString>(); }
    virtual QString name() const = 0;
    virtual int index() const { return 0; }
    virtual QVersitOrganizerHandler* createHandler() const = 0;

#ifdef Q_QDOC
    static const QLatin1Constant ProfileSync;
    static const QLatin1Constant ProfileBackup;
#else
    Q_DECLARE_LATIN1_CONSTANT(ProfileSync, "Sync");
    Q_DECLARE_LATIN1_CONSTANT(ProfileBackup, "Backup");
#endif
};

class Q_VERSIT_ORGANIZER_EXPORT QVersitOrganizerHandler : public QVersitOrganizerImporterPropertyHandler,
                                                public QVersitOrganizerExporterDetailHandler
{
public:
    virtual ~QVersitOrganizerHandler() {}
};

QTM_END_NAMESPACE

#define QT_VERSIT_ORGANIZER_HANDLER_INTERFACE "com.nokia.qt.mobility.versit.organizerhandlerfactory/1.0"
Q_DECLARE_INTERFACE(QtMobility::QVersitOrganizerHandlerFactory, QT_VERSIT_ORGANIZER_HANDLER_INTERFACE);

#endif
