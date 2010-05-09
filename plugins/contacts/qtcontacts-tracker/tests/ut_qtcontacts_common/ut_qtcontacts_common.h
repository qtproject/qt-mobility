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

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_COMMON_H
#define UT_QTCONTACTS_TRACKERPLUGIN_COMMON_H

#include <QContactAbstractRequest>
#include <QContactDetailFilter>
#include <QtTest/QtTest>

#define CHECK_CURRENT_TEST_FAILED                                               \
do {                                                                            \
    if (QTest::currentTestFailed()) {                                           \
        qWarning("failing test called from %s(%d)", __FILE__, __LINE__);    \
        return;                                                                 \
    }                                                                           \
} while (0)

QTM_USE_NAMESPACE

class ut_qtcontacts_common : public QObject
{
    Q_OBJECT

public:
    static const int DefaultTimeout = 5000;

    ut_qtcontacts_common();

private slots:
    // fixture setup
    virtual void initTestCase();
    virtual void cleanupTestCase();

    // function setup
    virtual void init();
    virtual void cleanup();

protected:
    QSet<QString> testSlotNames();

    void saveContact(QContact &contact, int timeout = DefaultTimeout);
    void saveContacts(QList<QContact> &contacts, int timeout = DefaultTimeout);
    void fetchContact(const QContactLocalId &id, QContact &result, int timeout = DefaultTimeout);
    void fetchContact(const QContactFilter &filter, QContact &result, int timeout = DefaultTimeout);
    void fetchContacts(const QList<QContactLocalId> &ids, QList<QContact> &result, int timeout = DefaultTimeout);
    void fetchContacts(const QContactFilter &filter, QList<QContact> &result, int timeout = DefaultTimeout);
    void waitForRequest(QContactAbstractRequest &request, int timeout = DefaultTimeout);

    QContactManager* mContactManager;
    QList<QContactLocalId> mLocalIds;
};

#endif /* UT_QTCONTACTS_TRACKERPLUGIN_COMMON_H */
