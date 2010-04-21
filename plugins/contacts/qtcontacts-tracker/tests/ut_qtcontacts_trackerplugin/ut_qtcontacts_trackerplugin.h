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

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_H
#define UT_QTCONTACTS_TRACKERPLUGIN_H

#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include <qcontactrequests.h>

QTM_BEGIN_NAMESPACE
class QContactLocalIdFetchRequest;
class QContactFetchRequest;
QTM_END_NAMESPACE

class QContactTrackerEngine;
QTM_USE_NAMESPACE

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
    void cleanup();
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
    void testSyncContactManagerContactsAddedSince();
    void testSyncTrackerEngineContactsIdsAddedSince();
    void testSyncContactManagerContactIdsAddedSince();
    void testContactsAddedSince();
    void testContactsModifiedSince();
    void testContactsRemovedSince();
//    void testGroupsAddedSince();
//    void testGroupsModifiedSince();
//    void testGroupsRemovedSince();
    void testNcoTypes();
    void testMergeTwoOnlineContacts();
    void testQRelationshipAndMergingContacts();
    void testAsyncReadContacts();
    void testFilterContacts();
    void testFilterContactsEndsWith();
    void testFilterTwoNameFields();
    void testTrackerUriToUniqueId();
    void testIMContactsAndMetacontactMasterPresence();
    void testIMContactsFilterring();
    void testContactsWithoutMeContact();
    void testDefinitionNames();

private:
    void syncContactsAddedSinceHelper(QDateTime& start, QList<QContactLocalId>& addedIds);

    void insertContact(const QString& contactURI, QContactLocalId uid, QString imId, QString imStatus, QString accountPath, QString protocol = "jabber");
    void updateIMContactStatus(const QString& uri, QString imStatus);
    QContact contact(QContactLocalId uid, QStringList detailsToLoad = QStringList());
    QList<QContact> contacts(QList<QContactLocalId> uids, QStringList detailsToLoad = QStringList());

private:
    QContactTrackerEngine *trackerEngine;
    QContactManager::Error error;
    QMap<int, QContactManager::Error>* errorMap;
    // Filtering and sort options used for QContactTrackerEngine.
    // Not used.
    QContactFilter queryFilter;
    QList<QContactSortOrder> sortOrders;
    QList<QContactLocalId> addedContacts;
};

class Slots: public QObject
{
    Q_OBJECT
public:
    QList<QContactLocalId> ids;
    QList<QContact> contacts;
public slots:
    void idResultsAvailable();
    void resultsAvailable();

};
#endif
