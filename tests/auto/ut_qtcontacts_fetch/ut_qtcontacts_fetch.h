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

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H
#define UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H

#include <QContact>
#include <QContactFetchRequest>
#include <QObject>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class ut_qtcontacts_fetch : public QObject
{
Q_OBJECT
public:
    ut_qtcontacts_fetch();
    ~ut_qtcontacts_fetch();

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

// Protected or public slots are _not_ called by the QTest framework.
protected slots:
    void onContactFetchRequestProgress();

    /** Do the next step, as dictated by the value of this->state.
     * This is a slot so it can be called in a timeout, to make sure that
     * we use the QContacts API semi-asynchronously.
     */
    void doNextOperation();

private:

    void removeContact();
    void addContact();
    void checkSavedContact();
    void fetchSavedForRemoval();

    QContactManager* getContactManager();

    /** Get the contact ID for the test contact if it exists already.
     * This then sets the next appropriate state and does the next step.
     *
     * @param any_contact Whether we should check for any contact at all. Otherwise check for the specific one.
     */
    void getExistingContact(bool any_contact = false);

    // wait (allowing the mainloop to respond) until this->waiting is false.
    bool waitForStop();

    QContact contact;
    QContactFetchRequest contactFetchRequest;

    bool waiting;

    // We must use async QtContacts API,
    // because the sync API is not supported with qtcontacts-tracker,
    // so this tells us what our callbacks should do next.
    enum State
    {
        STATE_START = 0,
        STATE_INITIAL_EXISTING_FETCHED, //Or not found.
        STATE_INITIAL_EXISTING_REMOVED,
        STATE_CONTACT_SAVED,
        STATE_AFTER_SAVE_FETCHED_FOR_CHECK, //Or not found.
        STATE_AFTER_SAVE_CHECKED,
        STATE_AFTER_SAVE_FETCHED_FOR_REMOVAL, //Or not found.
        STATE_SAVED_CONTACT_REMOVED
    };
    State state;



};

#endif /* UT_CONTACTSGUI_H_ */
