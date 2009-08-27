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

#include "batterycharge.h"

#include <qvaluespace.h>

QML_DEFINE_TYPE(Example, 1, 0, 0, BatteryCharge, BatteryCharge);

BatteryCharge::BatteryCharge(QObject *parent)
:   QObject(parent), item(0)
{
}

BatteryCharge::~BatteryCharge()
{
}

QString BatteryCharge::path() const
{
    return item ? item->itemName() : QString();
}

void BatteryCharge::setPath(const QString &path)
{
    delete item;
    item = new QValueSpaceItem(path);
    connect(item, SIGNAL(contentsChanged()), this, SIGNAL(chargeChanged()));
}

int BatteryCharge::charge() const
{
    return item ? item->value("", 0).toInt() : 0;
}
