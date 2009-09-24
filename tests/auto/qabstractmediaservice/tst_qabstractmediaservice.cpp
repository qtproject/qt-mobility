/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include "qabstractmediacontrol.h"
#include "qabstractmediaservice.h"

class QtTestMediaService;

class tst_QAbstractMediaService : public QObject
{
    Q_OBJECT
private slots:
    void control();

    void testEndpoints();

private:
    QtTestMediaService *m_service;
};

class QtTestMediaControlA : public QAbstractMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlA_iid "com.nokia.QtTestMediaControlA"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlA, QtTestMediaControlA_iid)

class QtTestMediaControlB : public QAbstractMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlB_iid "com.nokia.QtTestMediaControlB"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlB, QtTestMediaControlB_iid)

class QtTestMediaControlC : public QAbstractMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlC_iid "com.nokia.QtTestMediaControlC"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlC, QtTestMediaControlA_iid) // Yes A.

class QtTestMediaControlD : public QAbstractMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlD_iid "com.nokia.QtTestMediaControlD"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlD, QtTestMediaControlD_iid)


class QtTestMediaService : public QAbstractMediaService
{
    Q_OBJECT
public:
    QtTestMediaService()
        : QAbstractMediaService(0)
    {
    }

    QAbstractMediaControl* control(const char *name) const
    {
        if (strcmp(name, QtTestMediaControlA_iid) == 0)
            return const_cast<QtTestMediaControlA *>(&controlA);
        else if (strcmp(name, QtTestMediaControlB_iid) == 0)
            return const_cast<QtTestMediaControlB *>(&controlB);
        else if (strcmp(name, QtTestMediaControlC_iid) == 0)
            return const_cast<QtTestMediaControlC *>(&controlC);
        else
            return 0;
    }

    using QAbstractMediaService::control;

    QtTestMediaControlA controlA;
    QtTestMediaControlB controlB;
    QtTestMediaControlC controlC;
};

void tst_QAbstractMediaService::control()
{
    m_service = new QtTestMediaService;

    QCOMPARE(m_service->control<QtTestMediaControlA *>(), &m_service->controlA);
    QCOMPARE(m_service->control<QtTestMediaControlB *>(), &m_service->controlB);
    QVERIFY(!m_service->control<QtTestMediaControlC *>());  // Faulty implementation returns A.
    QVERIFY(!m_service->control<QtTestMediaControlD *>());  // No control of that type.
}

void tst_QAbstractMediaService::testEndpoints()
{
    QVERIFY(!m_service->isEndpointSupported(QAbstractMediaService::AudioInput));
    m_service->setInputStream((QIODevice*)0);
    QVERIFY(m_service->inputStream() == 0);
    m_service->setOutputStream((QIODevice*)0);
    QVERIFY(m_service->outputStream() == 0);
    QList<QString> endpoints = m_service->activeEndpoints(QAbstractMediaService::AudioInput);
    QVERIFY(endpoints.count() == 0);
    QVERIFY(!m_service->setActiveEndpoint(QAbstractMediaService::AudioInput, "test"));
    QVERIFY(m_service->endpointDescription(QAbstractMediaService::AudioInput, "test") == QString());
    endpoints = m_service->supportedEndpoints(QAbstractMediaService::AudioInput);
    QVERIFY(endpoints.count() == 0);
}

QTEST_MAIN(tst_QAbstractMediaService)

#include "tst_qabstractmediaservice.moc"
