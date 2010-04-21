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

#ifndef UT_CONTACTSGUI_EDITCONTACT_H_
#define UT_CONTACTSGUI_EDITCONTACT_H_

#include <QContact>
#include <QContactFetchRequest>
#include <QObject>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class ut_qtcontacts_add : public QObject
{
Q_OBJECT
public:
    ut_qtcontacts_add();
    ~ut_qtcontacts_add();

// Private slots are called by the QTest framework.
private slots:
    // Per test class:
    void initTestCase();
    void cleanupTestCase();

    // Per test-function:
    void init();
    void cleanup();

    // Test functions:
    void ut_testAddContact();

    void checkSubTypes();

// Protected or public slots are _not_ called by the QTest framework.
protected slots:
    void onContactFetchRequestProgress();
    void onContactFoundThenRemoveAndTest();
    void onTimeoutAddContact();
    void onContactFoundThenCheck();
    void onContactFoundThenRemoveAndStop();

private:

    QContactManager* getContactManager();

    typedef void (ut_qtcontacts_add::*FinishedCallbackFunc)(void);

    // Get the contact ID for the test contact if it exists already.
    void getExistingContact(FinishedCallbackFunc finishedCallback);

    // wait (allowing the mainloop to respond) until this->waiting is false.
    bool waitForStop();

    //A hacky way to bind an extra parameter to the Qt slot.
    FinishedCallbackFunc getExistingContactFinishedCallback;

    QContact contact;
    QContactFetchRequest contactFetchRequest;

    bool waiting;
};

#endif /* UT_CONTACTSGUI_H_ */
