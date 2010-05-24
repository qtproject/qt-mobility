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

#ifndef QVERSITRESOURCEHANDLER_H
#define QVERSITRESOURCEHANDLER_H

#include "qmobilityglobal.h"

#include <QString>
#include <QByteArray>

QTM_BEGIN_NAMESPACE

class QVersitDefaultResourceHandlerPrivate;
class QVersitProperty;

class QVersitResourceHandler
{
public:
    virtual ~QVersitResourceHandler() {}
    virtual bool loadResource(const QString& location, QByteArray* contents, QString* mimeType) = 0;
    virtual bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                              QString* location) = 0;
};

class QVersitDefaultResourceHandler : public QVersitResourceHandler
{
public:
    QVersitDefaultResourceHandler();
    virtual ~QVersitDefaultResourceHandler();
    virtual bool loadResource(const QString& location, QByteArray* contents, QString* mimeType);
    virtual bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                              QString* location);

protected:
    QVersitDefaultResourceHandlerPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITRESOURCEHANDLER_H
