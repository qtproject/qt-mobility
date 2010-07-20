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
#ifndef QMESSAGEADDRESS_H
#define QMESSAGEADDRESS_H
#include <QString>
#include <QList>
#include <qmessageglobal.h>

QTM_BEGIN_NAMESPACE

class QMessageAddressPrivate;

class Q_MESSAGING_EXPORT QMessageAddress
{
    friend class QMessageAddressPrivate;

public:
    enum Type {
        System = 1,
        Phone,
        Email,
        InstantMessage
        // Extensible
    };

    QMessageAddress();
    QMessageAddress(Type type, const QString &addressee);
    QMessageAddress(const QMessageAddress &other);

    virtual ~QMessageAddress();

    QMessageAddress& operator=(const QMessageAddress &other);

    bool operator==(const QMessageAddress &other) const;
    bool operator!=(const QMessageAddress &other) const;

    Type type() const;
    void setType(Type type);

    QString addressee() const;
    void setAddressee(const QString &addressee);

    static void parseEmailAddress(const QString& emailAddress, QString *name, QString *address, QString *suffix = 0, bool *startDelimeterFound = 0, bool *endDelimeterFound = 0);

private:
    QMessageAddressPrivate *d_ptr;
};

typedef QList<QMessageAddress> QMessageAddressList;

QTM_END_NAMESPACE

#endif
