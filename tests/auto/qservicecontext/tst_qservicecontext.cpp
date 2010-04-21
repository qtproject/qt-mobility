/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtCore>
#include <qservicecontext.h>

QTM_USE_NAMESPACE
class MyServiceContext : public QServiceContext
{
    Q_OBJECT
public:
    void notify(ContextType type, const QVariant &variant)
    {
        contextType = type;
        contextVariant = variant;
    }

    ContextType contextType;
    QVariant contextVariant;
};


class tst_QServiceContext: public QObject
{
    Q_OBJECT

private:
    void addStringData();

private slots:
    void clientId();
    void setClientId();
    void setClientId_data();

    void clientName();
    void setClientName();
    void setClientName_data();

    void notify();
};

void tst_QServiceContext::addStringData()
{
    QTest::addColumn<QString>("string");

    QTest::newRow("empty string") << QString();
    QTest::newRow("basic") << QString("abc");
    QTest::newRow("spaces") << QString("abc def");
    QTest::newRow("numbers") << QString("12312313 12312332");
}

void tst_QServiceContext::clientId()
{
    MyServiceContext c;
    QCOMPARE(c.clientId(), QString());
}

void tst_QServiceContext::setClientId()
{
    QFETCH(QString, string);

    MyServiceContext c;
    c.setClientId(string);
    QCOMPARE(c.clientId(), string);
}

void tst_QServiceContext::setClientId_data()
{
    addStringData();
}

void tst_QServiceContext::clientName()
{
    MyServiceContext c;
    QCOMPARE(c.clientName(), QString());
}

void tst_QServiceContext::setClientName()
{
    QFETCH(QString, string);

    MyServiceContext c;
    c.setClientName(string);
    QCOMPARE(c.clientName(), string);
}

void tst_QServiceContext::setClientName_data()
{
    addStringData();
}

void tst_QServiceContext::notify()
{
    MyServiceContext c;

    c.notify(QServiceContext::DisplayContext, "abc");
    QCOMPARE(c.contextType, QServiceContext::DisplayContext);
    QCOMPARE(c.contextVariant, qVariantFromValue<QString>("abc"));

    c.notify(QServiceContext::UserDefined, "123");
    QCOMPARE(c.contextType, QServiceContext::UserDefined);
    QCOMPARE(c.contextVariant, qVariantFromValue<QString>("123"));
}

QTEST_MAIN(tst_QServiceContext)

#include "tst_qservicecontext.moc"
