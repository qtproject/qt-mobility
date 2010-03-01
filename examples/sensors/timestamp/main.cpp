/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore>
#include <QtTest>


namespace theNameSpace {
    typedef quint64 qtimestamp;
}

Q_DECLARE_METATYPE(theNameSpace::qtimestamp)

using namespace theNameSpace;

//static int qtimestamp_id = qRegisterMetaType<qtimestamp>("qtimestamp");
static int theNameSpace_qtimestamp_id = qRegisterMetaType<qtimestamp>("theNameSpace::qtimestamp");



class propertyholder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(theNameSpace::qtimestamp timestamp READ timestamp)
public:
    qtimestamp timestamp() const
    {
        return static_cast<qtimestamp>(100);
    }
};


class timestamp : public QObject
{
    Q_OBJECT
private slots:
    void qvariant();
    void property();
};

void timestamp::qvariant()
{
    QVariant v;
    qtimestamp t;

    t = 100;
    v = t;

    qDebug() << "qMetaTypeId" << qMetaTypeId<qtimestamp>();
    qDebug() << "QMetaType::type" << QMetaType::type("theNameSpace::qtimestamp");
    qDebug() << "QVariant::type" << v.type();
    qDebug() << "QVariant::userType" << v.userType();
    qDebug() << "QVariant::typeName" << v.typeName();

    t = v.value<qtimestamp>();
    QVERIFY(t == 100);
}

void timestamp::property()
{
    QVariant v;
    qtimestamp t;

    QObject *o = new propertyholder;
    v = o->property("timestamp");

    qDebug() << "qMetaTypeId" << qMetaTypeId<qtimestamp>();
    qDebug() << "QMetaType::type" << QMetaType::type("theNameSpace::qtimestamp");
    qDebug() << "QVariant::type" << v.type();
    qDebug() << "QVariant::userType" << v.userType();
    qDebug() << "QVariant::typeName" << v.typeName();

    t = v.value<qtimestamp>();
    QVERIFY(t == 100);
}

QTEST_MAIN(timestamp)

#include "main.moc"
