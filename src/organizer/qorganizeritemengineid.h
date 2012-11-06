/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERITEMENGINEID_H
#define QORGANIZERITEMENGINEID_H

#include <QString>
#include <QSharedDataPointer>

#include "qtorganizerglobal.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

class Q_ORGANIZER_EXPORT QOrganizerItemEngineId: public QSharedData
{
public:
    virtual ~QOrganizerItemEngineId() {}

    virtual bool isEqualTo(const QOrganizerItemEngineId* other) const = 0;
    virtual bool isLessThan(const QOrganizerItemEngineId* other) const = 0;

    virtual QString managerUri() const = 0;
    virtual QOrganizerItemEngineId* clone() const = 0;

    virtual QString toString() const = 0;

#ifndef QT_NO_DEBUG_STREAM
    // NOTE: on platforms where Qt is built without debug streams enabled, vtable will differ!
    virtual QDebug& debugStreamOut(QDebug& dbg) const = 0;
#endif
    virtual uint hash() const = 0;
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemEngineId), Q_MOVABLE_TYPE);


#endif

