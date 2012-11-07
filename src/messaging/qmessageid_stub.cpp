/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageid.h"


QTM_BEGIN_NAMESPACE

QMessageId::QMessageId()
{
}

QMessageId::QMessageId(const QMessageId& other)
{
    Q_UNUSED(other)
}

QMessageId::QMessageId(const QString& id)
{
    Q_UNUSED(id)
}

QMessageId::~QMessageId()
{
}

QMessageId& QMessageId::operator=(const QMessageId& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageId::operator==(const QMessageId& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

bool QMessageId::operator<(const QMessageId& other) const
{
    return false; // stub
}

QString QMessageId::toString() const
{
    return QString::null; // stub
}

bool QMessageId::isValid() const
{
    return false; // stub
}

uint qHash(const QMessageId &id)
{
    Q_UNUSED(id)
    return 0; // stub
}

QTM_END_NAMESPACE

