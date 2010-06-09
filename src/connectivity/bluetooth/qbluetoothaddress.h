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

#ifndef QBLUETOOTHADDRESS_H
#define QBLUETOOTHADDRESS_H

#include "qmobilityglobal.h"

#include <QByteArray>
#include <QString>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Q_CONNECTIVITY_EXPORT QBluetoothAddress
{
public:
    QBluetoothAddress();
    explicit QBluetoothAddress(quint64 address);
    explicit QBluetoothAddress(const QString &address);
    QBluetoothAddress(const QBluetoothAddress &other);

    QBluetoothAddress &operator=(const QBluetoothAddress &other);

    bool isNull() const;

    void clear();

    bool operator<(const QBluetoothAddress &other) const;
    bool operator==(const QBluetoothAddress &other) const;
    inline bool operator!=(const QBluetoothAddress &other) const { return !operator==(other); }

    quint64 toUInt64() const;
    QString toString() const;

private:
    quint64 m_address;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
