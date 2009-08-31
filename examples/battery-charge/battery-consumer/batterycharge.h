/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
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
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BATTERYCHARGE_H
#define BATTERYCHARGE_H

#include <QObject>
#include <qml.h>

class QValueSpaceItem;

class BatteryCharge : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged);
    Q_PROPERTY(int charge READ charge NOTIFY chargeChanged);
    Q_PROPERTY(bool charging READ charging NOTIFY chargingChanged);

public:
    BatteryCharge(QObject *parent = 0);
    ~BatteryCharge();

    QString path() const;
    void setPath(const QString &path);

    int charge() const;

    bool charging() const;

signals:
    void pathChanged();
    void chargeChanged();
    void chargingChanged();

private slots:
    void contentsChanged();

private:
    QValueSpaceItem *m_item;
    bool m_charging;
    int m_charge;
};

QML_DECLARE_TYPE(BatteryCharge);

#endif // BATTERYCHARGE_H
