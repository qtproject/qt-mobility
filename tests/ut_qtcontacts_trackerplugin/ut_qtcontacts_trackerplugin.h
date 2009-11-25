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

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_H
#define UT_QTCONTACTS_TRACKERPLUGIN_H

#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include <qcontactrequests.h>

#include "qcontacttrackerbackend_p.h"
#include "qtrackercontactasyncrequest.h"

class QContactTrackerEngine;
/**
 * QtContacts Tracker plugin unittests
 */
class ut_qtcontacts_trackerplugin : public QObject
{
Q_OBJECT
public:
    ut_qtcontacts_trackerplugin();
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSavePhoneNumber();
    void testPhoneNumberContext();
    void testWritingOnlyWorkMobile();
    void testContacts();
    void testContact();
    void testAvatar();

    void testSaveEmailAddress();
    void testSaveName();
    void testSaveAddress();

    void testRemoveContact();
    void testSaveContacts();
    void testRemoveContacts();
    void testUrl();
//    void testGroups();
//    void testGroup();
//    void testSaveGroup();
//    void testRemoveGroup();
//    void testDetailDefinitions();
//    void testDetailDefinition();
//    void testSaveDetailDefinition();
//    void testRemoveDetailDefinition();
    void testContactsAddedSince();
    void testContactsModifiedSince();
    void testContactsRemovedSince();
//    void testGroupsAddedSince();
//    void testGroupsModifiedSince();
//    void testGroupsRemovedSince();
    void testNcoTypes();
    void testAsyncReadContacts();
    void testFilterContacts();
    void testFilterContactsEndsWith();
    void testTrackerUriToUniqueId();
    void testQRelationshipAndMetacontacts();

private:
    QContactTrackerEngine *trackerEngine;
    QContactManager::Error error;
    // Filtering and sort options used for QContactTrackerEngine.
    // Not used.
    QContactFilter queryFilter;
    QList<QContactSortOrder> sortOrders;
};

class Slots: public QObject
{
    Q_OBJECT
public:
    QList<QContactLocalId> ids;
    QList<QContact> contacts;
public slots:
    void progress(QContactLocalIdFetchRequest* self, bool appendOnly);
    void progress(QContactFetchRequest* self, bool appendOnly);
private:
    QString requestStatusToString(QContactAbstractRequest::Status status);
};
#endif
