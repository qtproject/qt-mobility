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

#include "ut_qtcontacts_trackerplugin.h"

#include <QMap>
#include <QPair>
#include <QUuid>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>
#include <qcontactfilters.h>
#include <QContactChangeLogFilter>
#include <qtcontacts.h>
#include <trackerchangelistener.h>
#include <qcontactrelationshipsaverequest.h>
#include <qcontactrelationshipfetchrequest.h>
#include <qtrackercontactidfetchrequest.h>
#include <qcontacttrackerbackend_p.h>
#include <qtrackercontactasyncrequest.h>

#include "contactmanager.h"

// update this when creating debian package
const QString PATH_TO_SPARQL_TESTS("./ut_qtcontacts_trackerplugin_data");

ut_qtcontacts_trackerplugin::ut_qtcontacts_trackerplugin()
{

}

void ut_qtcontacts_trackerplugin::initTestCase()
{
    QMap<QString, QString> trackerEngineParams;
    trackerEngine = new QContactTrackerEngine(trackerEngineParams);
    errorMap = new QMap<int, QContactManager::Error>();
}

void ut_qtcontacts_trackerplugin::testContacts()
{
    QContact c1;
    QContact c2;

    trackerEngine->saveContact(&c1, error);
    trackerEngine->saveContact(&c2, error);
    QVERIFY2((error == QContactManager::NoError),"Saving contact");
    QList<QContactLocalId> contacts = trackerEngine->contactIds(queryFilter, sortOrders, error);
    QVERIFY2(contacts.contains(c1.localId()), "Previously added contact is not found");
    QVERIFY2(contacts.contains(c2.localId()), "Previously added contact is not found");
}

void ut_qtcontacts_trackerplugin::testContact()
{
    // Test invalid contact id
    QContact invalidContact = trackerEngine->contact_impl( -1, QStringList(), error);
    QVERIFY(error != QContactManager::NoError);

    // Add a contact
    QContact newContact;
    const QContactLocalId oldid = newContact.localId();
    QVERIFY( trackerEngine->saveContact( &newContact, error ) );

    QContactLocalId id = newContact.localId();
    QVERIFY( id != oldid );

    // Find the added contact
    QContact c = trackerEngine->contact_impl( id, QStringList(), error );
    QVERIFY( c.localId() == newContact.localId() );
}

void ut_qtcontacts_trackerplugin::testSaveName()
{
    QContact c;
    QContactLocalId initialId = c.localId();
    int detailsAdded = 0;

    QMap<QString,QString> nameValues;
    QContactName name;
    nameValues.insert(QLatin1String(QContactName::FieldPrefix), "Mr");
    nameValues.insert(QLatin1String(QContactName::FieldFirst), "John");
    nameValues.insert(QLatin1String(QContactName::FieldMiddle), "Rupert");
    nameValues.insert(QLatin1String(QContactName::FieldLast), "Doe");
//    nameValues.insert(QContactName::FieldSuffix, "III");

    foreach (QString field, nameValues.keys()) {
        name.setValue(field, nameValues.value(field));
    }
    c.saveDetail(&name);

    QContactNickname nick;
    nick.setValue(QLatin1String(QContactNickname::FieldNickname), "Johnny");
    c.saveDetail(&nick);

    QVERIFY(c.detail<QContactName>().prefix() == "Mr");
    QVERIFY(c.detail<QContactName>().firstName() == "John");
    QVERIFY(c.detail<QContactName>().middleName() == "Rupert");
    QVERIFY(c.detail<QContactName>().lastName() == "Doe");
    QVERIFY(c.detail<QContactNickname>().nickname() == "Johnny");

    detailsAdded++;

    trackerEngine->saveContact(&c, error);
    QCOMPARE(error,  QContactManager::NoError);
    QVERIFY(c.localId() != initialId);
    QContact contact = this->contact(c.localId());
    QList<QContactName> details = contact.details<QContactName>();
    QList<QContactNickname> details2 = contact.details<QContactNickname>();
    QCOMPARE(details.count(), detailsAdded);
    QCOMPARE(details2.count(), detailsAdded);
    // Name is unique
    foreach(QString field, nameValues.keys()) {
        QCOMPARE(details.at(0).value(field), nameValues.value(field));
    }
    QCOMPARE(details2.at(0).value(QLatin1String(QContactNickname::FieldNickname)), QString("Johnny"));

    // Try changing the name of the saved contact.
    {
        QMap<QString,QString> nameValues;
        QContactName name = c.detail<QContactName>();
        nameValues.insert(QLatin1String(QContactName::FieldPrefix), "Mr2");
        nameValues.insert(QLatin1String(QContactName::FieldFirst), "John2");
        nameValues.insert(QLatin1String(QContactName::FieldMiddle), "Rupert2");
        nameValues.insert(QLatin1String(QContactName::FieldLast), "");
        //    nameValues.insert(QContactName::FieldSuffix, "III");

        foreach (QString field, nameValues.keys()) {
            name.setValue(field, nameValues.value(field));
        }
        c.saveDetail(&name);

        QContactNickname nick = c.detail<QContactNickname>();
        nick.setValue(QLatin1String(QContactNickname::FieldNickname), "Johnny2");
        c.saveDetail(&nick);


        QVERIFY(trackerEngine->saveContact(&c, error));
        QCOMPARE(error,  QContactManager::NoError);
        QVERIFY(c.localId() != initialId);

        QContact contact = trackerEngine->contact_impl(c.localId(), QStringList(), error);
        QCOMPARE(error,  QContactManager::NoError);
        QList<QContactName> details = contact.details<QContactName>();
        QList<QContactNickname> details2 = contact.details<QContactNickname>();
        QCOMPARE(details.count(), detailsAdded);
        QCOMPARE(details2.count(), detailsAdded);
        // Name is unique
        foreach(QString field, nameValues.keys()) {
            QCOMPARE(details.at(0).value(field), nameValues.value(field));
        }
        QCOMPARE(details2.at(0).value(QLatin1String(QContactNickname::FieldNickname)), QString("Johnny2"));

        // now try to add new name detail fails - this is how currently unique fields are implemented
        // so cover it in unit tests
        QContactName name1;
        name1.setValue(QContactName::FieldFirst, "Something that wont be stored as name is unique");
        c.saveDetail(&name1);
        // validate that unique name is not saved
        QVERIFY(!trackerEngine->saveContact(&c, error));
        details = contact.details<QContactName>();
        details2 = contact.details<QContactNickname>();
        QCOMPARE(details.count(), detailsAdded);
        QCOMPARE(details2.count(), detailsAdded);
    }
}

void ut_qtcontacts_trackerplugin::testSavePhoneNumber()
{
    // use the same values for 2 contacts
    for (int i = 0; i <2; i++ )
    {
    QContact c;
    QContactLocalId initialId = c.localId();
    int detailsAdded = 0;
    QContactName name;
    name.setFirstName("I have phone numbers");
    name.setLastName("Girl");
    c.saveDetail(&name);

    // key: phonenumber; value: context,subtype
    QMap<QString,QPair<QString,QString> > phoneValues;

    phoneValues.insert("(704)486-6472", QPair<QString,QString>(QLatin1String(QContactDetail::ContextHome), QString()));
    phoneValues.insert("(765)957-1663", QPair<QString,QString>(QLatin1String(QContactDetail::ContextHome), QString()));
    phoneValues.insert("(999)888-1111", QPair<QString,QString>(QLatin1String(QContactDetail::ContextHome),
                                                               QLatin1String(QContactPhoneNumber::SubTypeMobile)));

    phoneValues.insert("(792)123-6113", QPair<QString,QString>(QLatin1String(QContactDetail::ContextWork), QString()));
    phoneValues.insert("(918)491-7361", QPair<QString,QString>(QLatin1String(QContactDetail::ContextWork),
                                                  QLatin1String(QContactPhoneNumber::SubTypeMobile)));
    phoneValues.insert("(412)670-1514", QPair<QString,QString>(QLatin1String(QContactDetail::ContextWork),
                                                  QLatin1String(QContactPhoneNumber::SubTypeCar)));
    QMap<QString,QPair<QString,QString> > formattedPhoneValues;


    foreach (QString number, phoneValues.keys()) {
        QContactPhoneNumber phone;
        phone.setNumber(number);
        // Stripped automatically on saving RFC 3966 visual-separators reg exp "[(|-|.|)| ]"
        formattedPhoneValues.insert(QString(number).replace( QRegExp("[\\(|" \
                "\\-|" \
                "\\.|" \
                "\\)|" \
                " ]"), ""),phoneValues.value(number));
        if (!phoneValues.value(number).first.isEmpty()) {
            phone.setContexts(phoneValues.value(number).first);
        }
        if (!phoneValues.value(number).second.isEmpty()) {
            phone.setSubTypes(phoneValues.value(number).second);
        }
        c.saveDetail(&phone);
        detailsAdded++;
    }

    trackerEngine->saveContact(&c, error);
    QCOMPARE(error,  QContactManager::NoError);
    QVERIFY(c.localId() != initialId);
    // wait for commit transaction to be done, no signals yet
    for(int i = 0; i < 100; i++)
    {
        usleep(10000);
        QCoreApplication::processEvents();
    }


    // verify with synchronous read too
    QContact contact = trackerEngine->contact_impl(c.localId(), QStringList(), error);
    QCOMPARE(error,  QContactManager::NoError);
    QList<QContactPhoneNumber> details = contact.details<QContactPhoneNumber>();


    QCOMPARE(details.count(), detailsAdded);


    foreach (QContactPhoneNumber detail, details) {
        // Verify that the stored values and attributes are the same as given
        QVERIFY(formattedPhoneValues.contains(detail.number()));
        QCOMPARE(detail.contexts()[0], formattedPhoneValues.value(detail.number()).first);
        if( formattedPhoneValues.value(detail.number()).second.isEmpty()) // default empty is voice
            QCOMPARE(detail.subTypes()[0], QLatin1String(QContactPhoneNumber::SubTypeVoice));
        else
            QCOMPARE(detail.subTypes()[0], formattedPhoneValues.value(detail.number()).second);
    }

    // edit one of numbers . values, context and subtypes and save again
    QString editedPhoneValue = "+7044866473";
    QContactPhoneNumber phone = details[0];
    phone.setNumber(editedPhoneValue);
    phone.setContexts(QContactDetail::ContextWork);
    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c = contact;
    c.saveDetail(&phone);
    trackerEngine->saveContact(&c, error);
    QCOMPARE(error,  QContactManager::NoError);
    c = this->contact(c.localId(), QStringList()<<QContactPhoneNumber::DefinitionName);
    details = c.details<QContactPhoneNumber>();
    QCOMPARE(details.count(), detailsAdded);
    bool found = false;
    foreach (QContactPhoneNumber detail, details) {
        if(detail.number() == phone.number())
        {
            found = true;
            QVERIFY(detail.subTypes().contains(QContactPhoneNumber::SubTypeMobile));
            QVERIFY(detail.contexts().contains(QContactPhoneNumber::ContextWork));
            break;
        }
    }
    QVERIFY(found);
    }
}

void ut_qtcontacts_trackerplugin::testPhoneNumberContext()
{
    QContact c;
    QContactPhoneNumber phone;
    phone.setContexts(QContactDetail::ContextHome);
    phone.setNumber("555-888");
    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&phone);
    QContact contactToSave = c;
    // Let's do this all twice, first time save new detail, and next iteration change the context
    for (int iterations = 0; iterations < 2; iterations++) {
        QVERIFY(trackerEngine->saveContact(&contactToSave, error));
        // wait for commit transaction to be done, no signals yet
        for(int i = 0; i < 100; i++) {
            usleep(10000);
            QCoreApplication::processEvents();
        }

        QContactFetchRequest request;
        QContactLocalIdFilter filter;
        QList<QContactLocalId> ids;
        ids.append(contactToSave.localId());
        filter.setIds(ids);
        request.setFilter(filter);

        QStringList details;
        details << QContactPhoneNumber::DefinitionName;
        request.setDefinitionRestrictions(details);

        Slots slot;
        QObject::connect(&request, SIGNAL(resultsAvailable()),
                &slot, SLOT(resultsAvailable()));

        trackerEngine->startRequest(&request);

        for(int i = 0; i < 100; i++) {
            usleep(100000);
            QCoreApplication::processEvents();
            if(request.isFinished() )
                break;
        }

        // if it takes more, then something is wrong
        QVERIFY(request.isFinished());
        QVERIFY(!slot.contacts.isEmpty());

        QContact contactToTest;
        foreach (QContact savedContact, slot.contacts) {
            if (savedContact.localId() == contactToSave.localId()) {
                contactToTest = savedContact;
            }
        }
        QVERIFY(contactToTest.localId() == contactToSave.localId()); // Just to be sure we got the saved contact
        qDebug()<<contactToTest.details<QContactPhoneNumber>().count();
        
        QVERIFY(contactToTest.details<QContactPhoneNumber>().count() == 1);
        if (0 == iterations) {
            // perform context change
            QContactPhoneNumber phoneToEdit = contactToTest.detail<QContactPhoneNumber>();
            phoneToEdit.setContexts(QContactDetail::ContextWork);
            contactToTest.saveDetail(&phoneToEdit);
            contactToSave = contactToTest;
        }
        QVERIFY(contactToTest.details<QContactPhoneNumber>().count() == 1);
    }
}

void ut_qtcontacts_trackerplugin::testWritingOnlyWorkMobile()
{
    QContact c;
    QContactPhoneNumber phone;
    phone.setContexts(QContactDetail::ContextWork);
    phone.setNumber("555999");
    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&phone);
    QContact& contactToSave = c;
    QVERIFY(trackerEngine->saveContact(&contactToSave, error));
    // wait for commit transaction to be done, no signals yet
    for(int i = 0; i < 100; i++) {
        usleep(10000);
        QCoreApplication::processEvents();
    }

    QContactFetchRequest request;
    QContactLocalIdFilter filter;
    QList<QContactLocalId> ids;
    ids.append(contactToSave.localId());
    filter.setIds(ids);
    request.setFilter(filter);
    QStringList details;
    details << QContactPhoneNumber::DefinitionName;
    request.setDefinitionRestrictions(details);

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));

    trackerEngine->startRequest(&request);

    for(int i = 0; i < 100; i++) {
        usleep(100000);
        QCoreApplication::processEvents();
        if(request.isFinished() )
            break;
    }

    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QVERIFY(!slot.contacts.isEmpty());

    QContact contactToTest;
    foreach (QContact savedContact, slot.contacts) {
        if (savedContact.localId() == c.localId()) {
            contactToTest = savedContact;
        }
    }
    QVERIFY(contactToTest.localId() == c.localId()); // Just to be sure we got the saved contact
    QVERIFY(contactToTest.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(contactToTest.detail<QContactPhoneNumber>().number() == phone.number());
    QVERIFY(contactToTest.detail<QContactPhoneNumber>().subTypes() == phone.subTypes());
    QVERIFY(contactToTest.detail<QContactPhoneNumber>().contexts() == phone.contexts());
}

void ut_qtcontacts_trackerplugin::testSaveAddress()
{
    QContact c;
    QContactName name;
    name.setFirstName("Aruba & Barbados");
    name.setLastName("Girl");
    c.saveDetail(&name);
    QContactLocalId initialId = c.localId();
    int detailsAdded = 0;

    // List of pairs of field-value map and context
    typedef QMap<QString,QString> typeAddress;
    typedef QPair<typeAddress,QString> typeAddressWithContext;
    QList<typeAddressWithContext> addressValues;

    // TODO check status of 137174 and other libqttracker1pre6 bugs before refactoring
    typeAddress values;
    values.insert(QLatin1String(QContactAddress::FieldCountry), "Barbados");
    values.insert(QLatin1String(QContactAddress::FieldPostcode), "55555");
    values.insert(QLatin1String(QContactAddress::FieldStreet), "Martindales Rd");
    values.insert(QLatin1String(QContactAddress::FieldRegion), "Bridgetown");
    addressValues.append(typeAddressWithContext(values, QLatin1String(QContactDetail::ContextHome)));
    values.clear();
    values.insert(QLatin1String(QContactAddress::FieldCountry), "Aruba");
    values.insert(QLatin1String(QContactAddress::FieldPostcode), "44444");
    values.insert(QLatin1String(QContactAddress::FieldStreet), "Brazilie Straat");
    values.insert(QLatin1String(QContactAddress::FieldRegion), "Oranjestad");
    addressValues.append(typeAddressWithContext(values, QLatin1String(QContactDetail::ContextHome)));
    values.clear();
    values.insert(QLatin1String(QContactAddress::FieldCountry), "ArubaWork");
    values.insert(QLatin1String(QContactAddress::FieldPostcode), "44445");
    values.insert(QLatin1String(QContactAddress::FieldStreet), "Sunset Blvd");
    values.insert(QLatin1String(QContactAddress::FieldRegion), "Oranjestad");
    addressValues.append(typeAddressWithContext(values, QLatin1String(QContactDetail::ContextHome)));
    foreach (typeAddressWithContext addressWithContext, addressValues) {
        QContactAddress address;
        foreach (QString field, addressWithContext.first.keys()) {
            address.setValue(field, addressWithContext.first.value(field));
        }
        address.setContexts(addressWithContext.second);
        c.saveDetail(&address);
        detailsAdded++;
    }

    trackerEngine->saveContact(&c, error);
    QCOMPARE(error,  QContactManager::NoError);
    QVERIFY(c.localId() != initialId);
    QContact contact = trackerEngine->contact_impl(c.localId(), QStringList(), error);
    QList<QContactAddress> details = contact.details<QContactAddress>();
    QCOMPARE(details.count(), detailsAdded);
    bool found = false;
    // Test if inserted values are found in some of the details
    foreach (typeAddressWithContext addressWithContext, addressValues) {
        foreach (QContactAddress detail, details) {
            foreach (QString field, addressWithContext.first.keys()) {
                found = (detail.value(field) == addressWithContext.first.value(field));
                if (!found)
                    break;
            }
            if (found)
                break;
        }
        QVERIFY2(found, "Inserted detail was not found in the fetched details");
    }
}

void ut_qtcontacts_trackerplugin::testSaveEmailAddress()
{
    QContact c;
    QContactLocalId initialId = c.localId();
    int detailsAdded = 0;

    QMap<QString,QString> values;
    values.insert("john.does@hotmail.com", QContactDetail::ContextHome);
    values.insert("john.doe@gmail.com", QContactDetail::ContextWork);
    values.insert("john.doe@nokia.com", QContactDetail::ContextWork);
    values.insert("john.doe@johndoe.com", QContactDetail::ContextHome);
    foreach(QString address, values.keys()) {
        QContactEmailAddress emailAddress;
        emailAddress.setEmailAddress(address);
        emailAddress.setContexts(values.value(address));
        c.saveDetail(&emailAddress);
        detailsAdded++;
    }
    QContactName name;
    name.setFirstName("Jo");
    name.setLastName("H N Doe");
    c.saveDetail(&name);
    trackerEngine->saveContact(&c, error);
    QCOMPARE(error,  QContactManager::NoError);
    QVERIFY(c.localId() != initialId);
    QContact contact = trackerEngine->contact_impl(c.localId(), QStringList(), error);
    QList<QContactEmailAddress> details = contact.details<QContactEmailAddress>();
    QCOMPARE(details.count(), detailsAdded);
    foreach (QContactEmailAddress detail, details) {
        QString address = detail.value(QContactEmailAddress::FieldEmailAddress);
        QVERIFY(values.contains(address));
        QCOMPARE(detail.contexts()[0], values.value(address));
    }
}

void ut_qtcontacts_trackerplugin::testRemoveContact()
{
    QContact c;
    QContactPhoneNumber phone;
    phone.setNumber("+358501234567");
    c.saveDetail(&phone);
    QContactEmailAddress email;
    email.setEmailAddress("super.man@hotmail.com");
    c.saveDetail(&email);
    QContactName name;
    name.setFirstName("Super");
    name.setLastName("Man");
    c.saveDetail(&name);

    QVERIFY2(trackerEngine->saveContact(&c, error) && error == QContactManager::NoError, "Saving a contact failed");
    QVERIFY2(trackerEngine->removeContact(c.localId(), error), "Removing a contact failed");
    QCOMPARE(error, QContactManager::NoError);
    QVERIFY2(trackerEngine->contact_impl(c.localId(), QStringList(), error) == QContact(), "Found a contact, which should have been removed");
}

void ut_qtcontacts_trackerplugin::testSaveContacts()
{
    QList<QContact> contacts;
    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("John");
        name.setLastName(QString::number(i,10));
        c.saveDetail(&name);
        contacts.append(c);
    }

    QMap<int, QContactManager::Error>* errorMap;    
    trackerEngine->saveContacts(&contacts, errorMap, error);
    QCOMPARE(error, QContactManager::NoError);
    for (int i = 0; i < contacts.count(); i++) {
        QVERIFY(contacts[i].localId() != 0);
        QList<QContactName> details = trackerEngine->contact_impl(contacts[i].localId(), QStringList(), error).details<QContactName>();
        QVERIFY(details.count());
        QCOMPARE(details.at(0).lastName(),
                 QString("%1").arg(QString::number(i,10)));
    }
}

void ut_qtcontacts_trackerplugin::testRemoveContacts()
{
    QList<QContactLocalId> addedIds;
    for (int i = 0; i < 5; i++) {
        QContact c;
        QContactName name;
        name.setFirstName(QString("John%1").arg(QString::number(i,10)));
        c.saveDetail(&name);
        QVERIFY2(trackerEngine->saveContact(&c, error) && error == QContactManager::NoError, "Saving a contact failed");
        addedIds.append(c.localId());
    }
    QList<QContactLocalId> toApiRemove;
    toApiRemove.append(addedIds.takeLast());
    toApiRemove.append(addedIds.takeLast());
    QList<QContactLocalId> toPluginRemove(addedIds);
    // Remove all, but last of the added contacts
    bool success = trackerEngine->removeContacts(&toPluginRemove, errorMap, error);
    QCOMPARE(success, true);
    for (int i = 0; i < errorMap->count(); i++) {
        QVERIFY(toPluginRemove[i] == 0);
    }
    QCOMPARE(error, QContactManager::NoError);

    success = ContactManager::instance()->removeContacts(&toApiRemove, errorMap);
    QCOMPARE(success, true);
    for (int i = 0; i < errorMap->count(); i++) {
        QVERIFY(toApiRemove[i] == 0);
    }

    // Try to remove some previously removed contacts, but one valid contact
    success = trackerEngine->removeContacts(&addedIds, errorMap, error);
    QCOMPARE(errorMap->count(), addedIds.count());
    for (int i = 0; i < errorMap->count() - 1; i++) {
        QVERIFY2(addedIds[i] != 0, "Manager should not mark id as zero");
    }
}

void ut_qtcontacts_trackerplugin::testAvatar()
{
    QContact contactWithAvatar;
    QContactAvatar avatar;

    avatar.setAvatar("file:///home/user/.contacts/avatars/default_avatar.png");
    contactWithAvatar.saveDetail(&avatar);
    QContactName name;
    name.setFirstName("John");name.setLastName("A Frog");
    contactWithAvatar.saveDetail(&name);
    QVERIFY(trackerEngine->saveContact( &contactWithAvatar, error));

    QContact c = trackerEngine->contact_impl( contactWithAvatar.localId(), QStringList(), error);
    QList<QContactAvatar> avatars = c.details<QContactAvatar>();
    QVERIFY( avatars.size() );
    QCOMPARE( avatars[0].avatar(), avatar.avatar() );
}

void ut_qtcontacts_trackerplugin::testUrl()
{

    //Context home, homepage url
    QContact contactWithUrl1;
    QContactUrl url1;
    url1.setUrl("http://home.homepage");
    url1.setContexts(QContactDetail::ContextHome);
    url1.setSubType(QContactUrl::SubTypeHomePage);
    QContactName name;
    name.setFirstName("John");name.setLastName("TestUrl1");
    contactWithUrl1.saveDetail(&name);
    contactWithUrl1.saveDetail(&url1);
    QVERIFY(trackerEngine->saveContact(&contactWithUrl1, error));

    //Context work, homepage url
    QContact contactWithUrl2;
    QContactUrl url2;
    url2.setUrl("http://work.homepage");
    url2.setContexts(QContactDetail::ContextWork);
    url2.setSubType(QContactUrl::SubTypeHomePage);
    QContactName name2;
    name2.setLastName("TestUrl2");
    contactWithUrl2.saveDetail(&name2);
    contactWithUrl2.saveDetail(&url2);
    QVERIFY(trackerEngine->saveContact(&contactWithUrl2, error));

    //Context home, favourite url
    QContact contactWithUrl3;
    QContactUrl url3;
    url3.setUrl("http://home.favourite");
    url3.setContexts(QContactDetail::ContextHome);
    url3.setSubType(QContactUrl::SubTypeFavourite);

    name2.setLastName("TestUrl3");
    contactWithUrl3.saveDetail(&name2);
    contactWithUrl3.saveDetail(&url3);
    QVERIFY(trackerEngine->saveContact(&contactWithUrl3, error));


    QContactLocalId id1 = contactWithUrl1.localId();
    QContactLocalId id2 = contactWithUrl2.localId();
    QContactLocalId id3 = contactWithUrl3.localId();
    QCOMPARE(contact(id1).detail<QContactUrl>().url(), QString("http://home.homepage"));
    QCOMPARE(contact(id2).detail<QContactUrl>().url(), QString("http://work.homepage"));
    QCOMPARE(contact(id3).detail<QContactUrl>().url(), QString("http://home.favourite"));

    QVERIFY(contact(id1).detail<QContactUrl>().contexts()[0] == QContactDetail::ContextHome );
    QVERIFY(contact(id2).detail<QContactUrl>().contexts()[0] == QContactDetail::ContextWork );
    QVERIFY(contact(id3).detail<QContactUrl>().contexts()[0] == QContactDetail::ContextHome );

    QVERIFY(contact(id1).detail<QContactUrl>().subType() == QContactUrl::SubTypeHomePage );
    QVERIFY(contact(id2).detail<QContactUrl>().subType() == QContactUrl::SubTypeHomePage );
    QVERIFY(contact(id3).detail<QContactUrl>().subType() == QContactUrl::SubTypeFavourite );

}

/*
void ut_qtcontacts_trackerplugin::testGroups()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testGroup()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testSaveGroup()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testRemoveGroup()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testDetailDefinitions()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testDetailDefinition()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testSaveDetailDefinition()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testRemoveDetailDefinition()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}
*/

void ut_qtcontacts_trackerplugin::testSyncContactManagerContactsAddedSince()
{
    // FIXME move this code out: not supposed to compile in and load the same code as dll plugin
    QSKIP("Statically and dinamically linking the same code is not working", SkipAll);

    QDateTime start;
    QList<QContactLocalId> addedIds;
    syncContactsAddedSinceHelper(start, addedIds);

    QContactChangeLogFilter filter(QContactChangeLogFilter::EventAdded);
    filter.setSince(start);

    QList<QContactSortOrder> sortOrder;
    
    QList<QContact> contactIds = ContactManager::instance()->contacts( filter, sortOrder, QStringList() );
    qDebug() << "addedIds" << addedIds.size();
    qDebug() << "contactIds" << contactIds.size();
    QVERIFY2( contactIds.size() == addedIds.size(), "Incorrect number of filtered contacts");
}

void ut_qtcontacts_trackerplugin::testSyncTrackerEngineContactsIdsAddedSince()
{
    QDateTime start;
    QList<QContactLocalId> addedIds;
    syncContactsAddedSinceHelper(start, addedIds);

    QContactChangeLogFilter filter(QContactChangeLogFilter::EventAdded);
    filter.setSince(start);

    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;

    QList<QContactLocalId> contactIds = trackerEngine->contactIds( filter, sortOrder, error );
    qDebug() << "addedIds" << addedIds;
    qDebug() << "contactIds" << contactIds;
    QVERIFY2( contactIds.size() == addedIds.size(), "Incorrect number of filtered contacts");
}

void ut_qtcontacts_trackerplugin::testSyncContactManagerContactIdsAddedSince()
{
    // FIXME move this code out: not supposed to compile in and load the same code as dll plugin
    QSKIP("Statically and dinamically linking the same code is not working", SkipAll);
    QDateTime start;
    QList<QContactLocalId> addedIds;
    syncContactsAddedSinceHelper(start, addedIds);
    QContactChangeLogFilter filter(QContactChangeLogFilter::EventAdded);
    filter.setSince(start);
    QList<QContactSortOrder> sortOrder;


    QList<QContactLocalId> contactIds = ContactManager::instance()->contactIds(filter, sortOrder);
    qDebug() << "addedIds" << addedIds;
    qDebug() << "contactIds" << contactIds;
    QVERIFY2( contactIds.size() == addedIds.size(), "Incorrect number of filtered contacts");
}


void ut_qtcontacts_trackerplugin::syncContactsAddedSinceHelper(QDateTime& start, QList<QContactLocalId>& addedIds)
{
    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("A"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
    }

    QTest::qWait(1000);
    start = QDateTime::currentDateTime();

    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("B"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
        addedIds.append(c.localId());
    }
}

void ut_qtcontacts_trackerplugin::testContactsAddedSince()
{
    QList<QContactLocalId> addedIds;
    QDateTime start;
    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("A"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
    }

    QTest::qWait(1000);
    start = QDateTime::currentDateTime();

    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("B"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
        addedIds.append(c.localId());
    }

    // now one asynchronous request to read all the
    QContactFetchRequest request;
    QContactChangeLogFilter filter(QContactChangeLogFilter::EventAdded);
    filter.setSince(start);
    request.setFilter(filter);

    // here You specify which details are of interest
    QStringList details;
    details << QContactAvatar::DefinitionName
            << QContactBirthday::DefinitionName
            << QContactAddress::DefinitionName
            << QContactEmailAddress::DefinitionName
            << QContactDisplayLabel::DefinitionName
            << QContactGender::DefinitionName
            << QContactAnniversary::DefinitionName
            << QContactName::DefinitionName
            << QContactOnlineAccount::DefinitionName
            << QContactOrganization::DefinitionName
            << QContactPhoneNumber::DefinitionName;
    request.setDefinitionRestrictions(details);

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));

    // start. clients should, instead of following use
    // request.setManager(trackermanagerinstance);
    // request.start();
    trackerEngine->startRequest(&request);
    trackerEngine->waitForRequestFinished(&request, 10000);
    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QCOMPARE(slot.contacts.count(), addedIds.count());

    foreach(QContact cont, slot.contacts) {
        QVERIFY2(addedIds.contains(cont.localId()), "One of the added contacts was not reported as added");
    }

    QContactLocalIdFetchRequest idreq;
    filter.setSince(start);
    idreq.setFilter(filter);

    Slots slot2;
    QObject::connect(&idreq, SIGNAL(resultsAvailable()),
            &slot2, SLOT(idResultsAvailable()));
    trackerEngine->startRequest(&idreq);
    trackerEngine->waitForRequestFinished(&idreq, 10000);
    QVERIFY(idreq.isFinished());
    QCOMPARE(slot2.ids.count(), addedIds.count());
    foreach(QContactLocalId id, slot2.ids) {
        QVERIFY2(addedIds.contains(id), "One of the added contacts was not reported as added");
    }

}

void ut_qtcontacts_trackerplugin::testContactsModifiedSince()
{
    QDateTime start;
    QList<QContactLocalId> addedIds;
    QList<QContactLocalId> modified;

    const int contactsToAdd = 5;
    const int contactsToModify = 3;
    QVERIFY2(contactsToAdd >= contactsToModify, "Cannot modify more contacts than this test has added");
    QVERIFY2(contactsToModify+1 <= contactsToAdd, "Cannot modify more contacts than this test has added");

    // Add contacts with only first name and store them to list of added
    for (int i = 0; i < contactsToAdd; i++) {
        QContact c;
        QContactName name;
        name.setFirstName("A"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
        addedIds.append(c.localId());
    }

    QTest::qWait(2000);
    start = QDateTime::currentDateTime();

   // Modify and save rest of the contacts
    for (int i = 0; i < contactsToModify; i++) {
        QContact c = contact(addedIds[i]);
        QContactName name = c.detail<QContactName>();
        // Modify name
        name.setFirstName("B"+QString::number(i));
        QVERIFY2(c.saveDetail(&name), "Failed to save detail");
        QVERIFY2(trackerEngine->saveContact(&c, error), "Failed to save contact");
        modified.append(c.localId());
    }
    // Set filter
    QContactChangeLogFilter filter(QContactChangeLogFilter::EventChanged);
    filter.setSince(start);

    QContactLocalIdFetchRequest idfetch;
    QContactFetchRequest fetch;
    idfetch.setFilter(filter);
    fetch.setFilter(filter);
    trackerEngine->startRequest(&idfetch);
    trackerEngine->waitForRequestFinished(&idfetch, 10000);
    QVERIFY2(idfetch.isFinished(), "Id fetch request did not finish on time");
    QVERIFY2(idfetch.error() == QContactManager::NoError, "Id fetch request finished with errors");
    QList<QContactLocalId> actuallyModifiedIds = idfetch.ids();
    trackerEngine->startRequest(&fetch);
    trackerEngine->waitForRequestFinished(&fetch, 10000);
    QVERIFY2(fetch.isFinished(), "Fetch request did not finish on time");
    QVERIFY2(fetch.error() == QContactManager::NoError, "Fetch request finished with errors");
    QList<QContact> actuallyModified = fetch.contacts();

    // Num of actually modified should be same as supposedly modified
    QCOMPARE(actuallyModifiedIds.count(), modified.count());
    QCOMPARE(actuallyModified.count(), modified.count());
    // All the ids of the modified contacts should be found in the result list
    foreach (QContactLocalId id, modified) {
        QVERIFY2(actuallyModifiedIds.contains(id), "One the modified contacts was not reported as modified");
    }
}

void ut_qtcontacts_trackerplugin::testContactsRemovedSince()
{
    QDateTime start = QDateTime::currentDateTime();
    QContactChangeLogFilter filter(QContactChangeLogFilter::EventRemoved);
    filter.setSince(start);
    QList<QContactSortOrder> sorts;
    QList<QContactLocalId> actuallyRemoved = trackerEngine->contactIds(filter, sorts, error);
    QVERIFY(actuallyRemoved.isEmpty());
    QVERIFY(error == QContactManager::NotSupportedError);
}
/*
void ut_qtcontacts_trackerplugin::testGroupsAddedSince()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testGroupsModifiedSince()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}

void ut_qtcontacts_trackerplugin::testGroupsRemovedSince()
{
    qDebug() << "Not implemented";
    QVERIFY(false);
}
*/

void ut_qtcontacts_trackerplugin::cleanupTestCase()
{
    delete trackerEngine;
    delete errorMap;
}

void ut_qtcontacts_trackerplugin::cleanup()
{
    foreach (QContactLocalId id, addedContacts) {
        trackerEngine->removeContact(id, error);
    }
    addedContacts.clear();
}


void ut_qtcontacts_trackerplugin::testNcoTypes()
{
    using namespace SopranoLive;

    QList<QContactLocalId> ids;
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    query.addColumn("contactId", RDFContact.property<nco::contactUID>());
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    foreach( Live<nco::PersonContact> p, ncoContacts ) {
        QVERIFY(p.hasType<nco::Contact>());
        QVERIFY(p.hasType<nco::Role>());
        QVERIFY(p.hasType<nco::PersonContact>());
    }
}

void ut_qtcontacts_trackerplugin::testAsyncReadContacts()
{
    addedContacts.clear();
    // Add at least one contact to be sure that this doesn't fail because tracker is clean

    QStringList firstNames, lastNames;
    firstNames << "aa" << "ab" << "ac" << "dd" << "fe";
    lastNames << "fe" << "ab" << "dd" << "dd" << "aa";
    for (int i = 0; i < firstNames.count(); i++) {
        QContact c;
        QContactName name;
        name.setFirstName(firstNames.at(i));
        name.setLastName(lastNames.at(i));
        QContactAvatar avatar;
        avatar.setAvatar("default_avatar.png");
        avatar.setSubType(QContactAvatar::SubTypeImage);
        QVERIFY(c.saveDetail(&name));
        QVERIFY(c.saveDetail(&avatar));
        QVERIFY(trackerEngine->saveContact(&c, error));
        addedContacts.append(c.localId());
    }
    
    // Prepare the filter for the request - we really should test only the contact we add here.
    QContactLocalIdFilter filter;
    filter.setIds(addedContacts);

    // this one will get complete contacts

    Slots slot;
    QContactFetchRequest request;
    QList<QContactSortOrder> sorting;
    QContactSortOrder sort, sort1;
    sort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    sort1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    sorting << sort << sort1;
    QStringList details; details << QContactName::DefinitionName << QContactAvatar::DefinitionName;
    request.setDefinitionRestrictions(details);
    request.setSorting(sorting);
    request.setFilter(filter);

    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));

    // this one only ids
    QContactLocalIdFetchRequest request1;
    request1.setFilter(filter);
    QObject::connect(&request1, SIGNAL(resultsAvailable()),
            &slot, SLOT(idResultsAvailable()));

    // the purpose is to compare if all contacts are loaded, and
    // if optional fields are defined properly in request

    // start both at once
    trackerEngine->startRequest(&request);
    trackerEngine->startRequest(&request1);
    trackerEngine->waitForRequestFinished(&request, 10000);
    trackerEngine->waitForRequestFinished(&request1, 10000);


    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QVERIFY(request1.isFinished());

    // there need1 to be something added to be verified
    QVERIFY(!request.contacts().isEmpty());
    // now ask for one contact
    QVERIFY(!slot.contacts.isEmpty());
    // there need to be something added to be verified
    QVERIFY(!request1.ids().isEmpty());
    // now ask for one contact
    QVERIFY(!slot.ids.isEmpty());

    QVERIFY2(slot.contacts.count() == slot.ids.count(), "not all contacts were loaded");
    QVERIFY(slot.contacts.count() >= firstNames.count());
    for( int i = 0; i < slot.contacts.size() -1 ; i++)
    {
        QContact contact = slot.contacts[i];
        QContact contact1 = slot.contacts[i+1];
        QString last0 = contact.detail<QContactName>().lastName();
        QString first0 = contact.detail<QContactName>().firstName();
        QString last1 = contact1.detail<QContactName>().lastName();
        QString first1 = contact1.detail<QContactName>().firstName();
        // sorting
        qDebug() << "contacts:" << contact.localId() << first0 << last0;
        bool test = last0 < last1 || (last0 == last1 && first0 <= first1);
        if (!test) {
            qDebug() << "contacts sort failed. First: " << contact1.localId() << first0 << last1 << "lasts: " << last0 << last1;
        }
        QVERIFY2(test, "Sorting failed.");
    }

}

void ut_qtcontacts_trackerplugin::testFilterContacts()
{
    // this one will get complete contacts
    QContact c;
    QContactName name;
    name.setFirstName("Zuba");
    name.setLastName("Zub");
    c.saveDetail(&name);
    QContactPhoneNumber phone;

    phone.setNumber("4872444");
    c.saveDetail(&phone);

    QContactBirthday birthday;
    birthday.setDate(QDate(2010, 2, 14));
    c.saveDetail(&birthday);

    trackerEngine->saveContact(&c, error);

    QStringList details;
    details << QContactName::DefinitionName << QContactAvatar::DefinitionName
            << QContactPhoneNumber::DefinitionName;

    QContactFetchRequest request;
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));
    filter.setValue(QString("4872444"));
    filter.setMatchFlags(QContactFilter::MatchEndsWith);

    request.setDefinitionRestrictions(details);
    request.setFilter(filter);

    trackerEngine->startRequest(&request);

    for(int i = 0; i < 100; i++)
    {
        usleep(100000);
        QCoreApplication::processEvents();
        if(request.isFinished() )
            break;
    }

    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QVERIFY(!request.contacts().isEmpty());

    QVERIFY(!slot.contacts.isEmpty());

    bool containsThisId = false;
    foreach(const QContact &contact, slot.contacts)
    {
        if( contact.localId() == c.localId())
            containsThisId = true;
        bool containsPhone = false;
        foreach(const QContactDetail &detail, contact.details(QContactPhoneNumber::DefinitionName))
        {
            if(detail.value(QContactPhoneNumber::FieldNumber).contains("4872444"))
            {
                containsPhone = true;
                break;
            }
        }
        QVERIFY(containsPhone);
    }
    QVERIFY(containsThisId);

    // filter by birthday range
    QContactDetailRangeFilter rangeFilter;
    rangeFilter.setDetailDefinitionName(QContactBirthday::DefinitionName, QContactBirthday::FieldBirthday);
    // include lower & exclude upper by default
    rangeFilter.setRange(QDate(2010, 2, 14), QDate(2010, 2, 15));
    QList<QContact> contacts = trackerEngine->contacts(rangeFilter, QList<QContactSortOrder>(), QStringList()<< QContactBirthday::DefinitionName, error);
    QVERIFY(!contacts.isEmpty());
    bool containsOurContact(false);
    foreach(const QContact &cont, contacts)
    {
        QVERIFY(cont.detail<QContactBirthday>().date() == QDate(2010, 2, 14));
        if( c.id() == cont.id() )
            containsOurContact = true;
    }
    QVERIFY(containsOurContact);
}

void ut_qtcontacts_trackerplugin::testFilterContactsEndsWith()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
    QString restoreValue = settings.value("phoneNumberMatchDigitCount", "7").toString();

    QContact matchingContact;
    QContactName name;
    name.setFirstName("Zuba");
    name.setLastName("Zub");
    matchingContact.saveDetail(&name);
    QContactPhoneNumber phone;
    // TODO doesnt work yet phone.setContexts(QContactPhoneNumber::ContextWork);
    phone.setNumber("3210987654321");
    matchingContact.saveDetail(&phone);
    trackerEngine->saveContact(&matchingContact, error);

    QStringList details;
    details << QContactName::DefinitionName << QContactAvatar::DefinitionName
            << QContactPhoneNumber::DefinitionName;

    QContactFetchRequest request;
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));

    {
        // test matching of 7 last digits
        int matchCount = 7;
        qDebug() << "Test matching of" << matchCount << "last digits.";
        settings.setValue("phoneNumberMatchDigitCount", matchCount);
        QString matchValue = "3000007654321";
        QContact nonMatchingContact;
        nonMatchingContact.saveDetail(&name);
        phone.setNumber("3210980654321");
        nonMatchingContact.saveDetail(&phone);
        trackerEngine->saveContact(&nonMatchingContact, error);

        filter.setValue(matchValue);
        filter.setMatchFlags(QContactFilter::MatchEndsWith);

        request.setDefinitionRestrictions(details);
        request.setFilter(filter);

        trackerEngine->startRequest(&request);

        for(int i = 0; i < 100; i++) {
            usleep(100000);
            QCoreApplication::processEvents();
            if (request.isFinished())
                break;
        }
        QVERIFY(request.isFinished());
        QVERIFY(!slot.contacts.isEmpty());

        bool containsMatchingId = false;
        bool containsNonMatchingId = false;
        foreach(const QContact &contact, slot.contacts) {
            if (contact.localId() == nonMatchingContact.localId())
                containsNonMatchingId = true;
            if (contact.localId() == matchingContact.localId())
                containsMatchingId = true;
            bool containsPhone = false;
            foreach(const QContactDetail &detail, contact.details(QContactPhoneNumber::DefinitionName)) {
                if (detail.value(QContactPhoneNumber::FieldNumber).endsWith(matchValue.right(matchCount))) {
                    containsPhone = true;
                    break;
                }
            }
            QVERIFY(containsPhone);
        }
        QVERIFY(containsMatchingId);
        QVERIFY(!containsNonMatchingId);
    }

    {
        // test matching of 11 last digits
        int matchCount = 11;
        qDebug() << "Test matching of" << matchCount << "last digits.";
        settings.setValue("phoneNumberMatchDigitCount", matchCount);
        QString matchValue = "3010987654321";
        QContact nonMatchingContact;
        nonMatchingContact.saveDetail(&name);
        phone.setNumber("3200987654321");
        nonMatchingContact.saveDetail(&phone);
        trackerEngine->saveContact(&nonMatchingContact, error);

        QContact matchingContactWithShorterNumber;
        QContactName name1;
        name1.setFirstName("ShortNumber");
        name1.setLastName("Zub1");
        matchingContactWithShorterNumber.saveDetail(&name1);
        QContactPhoneNumber phone1;
        phone1.setNumber("54321");
        matchingContactWithShorterNumber.saveDetail(&phone1);
        trackerEngine->saveContact(&matchingContactWithShorterNumber, error);
        QVERIFY(QContactManager::NoError == error);


        filter.setValue(matchValue);
        filter.setMatchFlags(QContactFilter::MatchEndsWith);

        request.setDefinitionRestrictions(details);
        request.setFilter(filter);

        trackerEngine->startRequest(&request);

        for(int i = 0; i < 100; i++) {
            usleep(100000);
            QCoreApplication::processEvents();
            if (request.isFinished())
                break;
        }
        QVERIFY(request.isFinished());
        QVERIFY(!slot.contacts.isEmpty());

        bool containsMatchingId = false;
        bool containsNonMatchingId = false;
        foreach(const QContact &contact, slot.contacts) {
            if (contact.localId() == nonMatchingContact.localId())
                containsNonMatchingId = true;
            if (contact.localId() == matchingContact.localId())
                containsMatchingId = true;
            bool containsPhone = false;
            foreach(const QContactDetail &detail, contact.details(QContactPhoneNumber::DefinitionName)) {
                if (detail.value(QContactPhoneNumber::FieldNumber).endsWith(matchValue.right(matchCount))) {
                    containsPhone = true;
                    break;
                }
            }
            QVERIFY(containsPhone);
        }
        QVERIFY(containsMatchingId);
        QVERIFY(!containsNonMatchingId);

        // now verify with short number
        filter.setValue("54321");
        filter.setMatchFlags(QContactFilter::MatchEndsWith);

        request.setDefinitionRestrictions(details);
        request.setFilter(filter);

        trackerEngine->startRequest(&request);

        for(int i = 0; i < 100; i++) {
            usleep(100000);
            QCoreApplication::processEvents();
            if (request.isFinished())
                break;
        }
        QVERIFY(request.isFinished());
        QVERIFY(!slot.contacts.isEmpty());
        bool containsShort = false;
        foreach(const QContact &contact, slot.contacts) {
            if (contact.localId() == matchingContactWithShorterNumber.localId())
                containsShort = true;
        }
        QVERIFY(containsShort);
    }
    settings.setValue("phoneNumberMatchDigitCount", restoreValue);
}

void ut_qtcontacts_trackerplugin::testFilterTwoNameFields()
{
    // init test
    QMap<QContactLocalId, QContactName> names;
    for (int i = 0; i < 3; i++) {
        QContact c;
        QContactName name;
        name.setFirstName(QUuid::createUuid().toString() + QString::number(i));
        name.setLastName(QUuid::createUuid().toString() + QString::number(i));
        c.saveDetail(&name);
        QContactAvatar avatar;
        avatar.setAvatar(QUuid::createUuid().toString());
        c.saveDetail(&avatar);
        QVERIFY(trackerEngine->saveContact(&c, error));        
        names.insert(c.localId(), name);
        QCOMPARE(error, QContactManager::NoError);
        addedContacts.append(c.localId());
    }

    // Init filter
    QContactLocalId searchId = names.keys().at(1);
    QString searchFirst = names.value(searchId).firstName();
    QString searchLast = names.value(searchId).lastName();
    QContactUnionFilter ufilter;
    QContactDetailFilter filterFirst;
    filterFirst.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    filterFirst.setMatchFlags(QContactFilter::MatchExactly);
    filterFirst.setValue(searchFirst);
    QContactDetailFilter filterLast;
    filterLast.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    filterLast.setMatchFlags(QContactFilter::MatchExactly);
    filterLast.setValue(searchLast);
    ufilter.setFilters(QList<QContactFilter>() << filterFirst << filterLast);

    // Init request
    QContactFetchRequest request;
    request.setFilter(ufilter);
    trackerEngine->startRequest(&request);
    trackerEngine->waitForRequestFinished(&request, 10000);


    // Test fetch result
    QCOMPARE(request.contacts().count(), 1);
    QCOMPARE(request.contacts().at(0).localId(), searchId);
    QCOMPARE(request.contacts().at(0).detail<QContactName>().firstName(), searchFirst);
    QCOMPARE(request.contacts().at(0).detail<QContactName>().lastName(), searchLast);
}

void ut_qtcontacts_trackerplugin::testTrackerUriToUniqueId()
{
    QString uri = "contact:1234567";
    QContactLocalId id = url2UniqueId( uri );
    QCOMPARE( (int)id, 1234567 );
}

void ut_qtcontacts_trackerplugin::testQRelationshipAndMergingContacts()
{
    QContact firstContact;
    QContactName name;
    name.setFirstName("FirstMeta");
    firstContact.saveDetail(&name);
    QVERIFY(trackerEngine->saveContact(&firstContact, error));

    QList<QContactLocalId> secondIds;
    QStringList names(QStringList()<<"SecondMeta"<<"ThirdMeta");
    foreach (QString firstname, names)
    {
        QContact secondContact;
        QContactName name1;
        name1.setFirstName(firstname);
        secondContact.saveDetail(&name1);
        QVERIFY(trackerEngine->saveContact(&secondContact, error));
        secondIds<<secondContact.id().localId();
        QContactRelationship rel;
        rel.setRelationshipType(QContactRelationship::Is);
        rel.setFirst(firstContact.id());
        rel.setSecond(secondContact.id());
        QContactRelationshipSaveRequest req;
        req.setRelationships(QList<QContactRelationship>()<<rel);
        QVERIFY(trackerEngine->startRequest(&req));
        trackerEngine->waitForRequestFinished(&req, 10000);
        // if it takes more, then something is wrong
        QVERIFY(req.isFinished());
        QVERIFY(QContactManager::NoError == req.error());
    }

    // once they are merged - that's it - no contacts or relationship track exists
    foreach( QContactLocalId mergedId, secondIds)
    {
        QContact second = contact(mergedId, QStringList()<<QContactName::DefinitionName);
        QVERIFY(second.localId() == 0); // as not existing
    }
    QVERIFY(contact(firstContact.localId(), QStringList()<<QContactName::DefinitionName).localId() != 0);
    // TODO check that values from secondids are merged to firstcontact

}

void ut_qtcontacts_trackerplugin::insertContact(const QString& URI, QContactLocalId uid, QString imId, QString imStatus, QString accountPath, QString protocol )
{
    QProcess inserter;
    QStringList args;
    args << URI << QString::number(uid) << imId << accountPath << imStatus << "In Helsinki" << protocol << "Some" << "Guy";
    inserter.start( PATH_TO_SPARQL_TESTS+"/insertTpContact.sparql", args );
    inserter.waitForFinished();
}

void ut_qtcontacts_trackerplugin::updateIMContactStatus(const QString& uri, QString imStatus)
{
    QProcess inserter;
    QStringList args;
    args << uri << imStatus;
    inserter.start( PATH_TO_SPARQL_TESTS+"/updateTpStatus.sparql", args );
    inserter.waitForFinished();
}

void ut_qtcontacts_trackerplugin::testMergeTwoOnlineContacts()
{
    QList<QContact> contacts;
    for( int i = 0; i < 3; i++ )
    {
        unsigned int contactid = 555+i;
        insertContact(QString("contact:") + QString::number(contactid),
            contactid, QString::number(contactid)+ "@ovi.com", "nco:presence-status-available", QString("/org/freedesktop/fake/account/%1").arg(contactid),"ovi.com");
        QContact c = contact(contactid, QStringList()<<QContactOnlineAccount::DefinitionName);
        contacts << c;
    }
    QContactRelationship rel;
    rel.setFirst(contacts.at(0).id());
    rel.setSecond(contacts.at(1).id());
    QContactRelationship rel2;
    rel2.setFirst(contacts.at(0).id());
    rel2.setSecond(contacts.at(2).id());
    
    QContactRelationshipSaveRequest req;
    //TODO adding rel2 to the following causes segfault
    req.setRelationships(QList<QContactRelationship>()<<rel);
    QVERIFY(trackerEngine->startRequest(&req));
    trackerEngine->waitForRequestFinished(&req, 1000);
    QVERIFY(req.isFinished());
    QVERIFY(QContactManager::NoError == req.error());
    
    QList<QContactOnlineAccount> onlineAccounts = contacts.at(0).details<QContactOnlineAccount>();
    qDebug() << onlineAccounts.size();
    QEXPECT_FAIL("", "Do net yet support merging multiple im contacts", Continue);
    QVERIFY(onlineAccounts.size() == 2);
}

void ut_qtcontacts_trackerplugin::testIMContactsAndMetacontactMasterPresence()
{
    if( !QFileInfo(PATH_TO_SPARQL_TESTS).exists() )
    {
        qWarning()<<Q_FUNC_INFO<<"is disabled - test scripts are not installed";
        return;
    }
    QList<unsigned int> idstomerge;
    QContactLocalId masterContactId; // using one master contact later for additional testing
    for( int i = 0; i < 2; i++ )
    {
        unsigned int contactid = 999998+i;
        idstomerge << contactid;
        insertContact(QString("contact:") + QString::number(999998+i),
                contactid, QString::number(999998 + i)+ "@ovi.com", "nco:presence-status-available", QString("/org/freedesktop/fake/account/%1").arg(999998+i),"ovi.com");
        QContact c = contact(contactid, QStringList()<<QContactOnlineAccount::DefinitionName);
        QVERIFY(c.localId() == contactid);
        QVERIFY(c.detail<QContactOnlineAccount>().serviceProvider() == "ovi.com");
        QContact firstContact;
        QContactName name;
        name.setFirstName("FirstMetaWithIM"+QString::number(contactid));
        firstContact.saveDetail(&name);
        QVERIFY(trackerEngine->saveContact(&firstContact, error));

        // save metarelationship
        QContactRelationship rel;
        rel.setRelationshipType(QContactRelationship::Is);
        rel.setFirst(firstContact.id());
        masterContactId = firstContact.localId();
        rel.setSecond(c.id());
        QContactRelationshipSaveRequest req;
        req.setRelationships(QList<QContactRelationship>()<<rel);
        QVERIFY(trackerEngine->startRequest(&req));
        trackerEngine->waitForRequestFinished(&req, 1000);
        QVERIFY(req.isFinished());
        QVERIFY(QContactManager::NoError == req.error());
    }

    // expected behavior - is that master contact contains all details aggregated
    {
        QList<QContact> cons = contacts(QList<QContactLocalId> ()
                << masterContactId << 999999, QStringList()
                << QContactOnlineAccount::DefinitionName);
        QVERIFY(cons.size() == 1);
        QVERIFY(cons[0].id().localId() == masterContactId);

        bool containDetail = false;
        foreach(QContactOnlineAccount det, cons[0].details<QContactOnlineAccount>())
            {
                if (det.value("Account") == "999999@ovi.com" // deprecated, going to account URI
                        || det.accountUri() == "999999@ovi.com")
                {
                    QVERIFY(det.presence() == QContactOnlineAccount::PresenceAvailable);
                    containDetail = true;
                }
            }
        QVERIFY(containDetail);
    }
    //now update presence to IM Address and check it in contact (TODO and if signal is emitted)
    updateIMContactStatus("telepathy:/org/freedesktop/fake/account/999999/999999@ovi.com", "nco:presence-status-offline");
    {
        QList<QContact> cons = contacts(QList<QContactLocalId> ()
                << masterContactId << 999999, QStringList()
                << QContactOnlineAccount::DefinitionName);
        QVERIFY(cons.size() == 1);
        QVERIFY(cons[0].id().localId() == masterContactId);

        bool containDetail = false;
        foreach(QContactOnlineAccount det, cons[0].details<QContactOnlineAccount>())
            {
                if (det.value("Account") == "999999@ovi.com" // deprecated, going to account URI
                        || det.accountUri() == "999999@ovi.com")
                {
                    QVERIFY(det.presence() == QContactOnlineAccount::PresenceOffline);
                    containDetail = true;
                }
            }
        QVERIFY(containDetail);
    }

    // load contact should load also all merged content from other contacts (that dont exis anymore)
    {
        QList<QContact> cons = contacts(QList<QContactLocalId> ()
                << masterContactId, QStringList()
                << QContactOnlineAccount::DefinitionName);
        QVERIFY(cons.size() == 1);
        QVERIFY(cons[0].id().localId() == masterContactId);

        bool containDetail = false;
        foreach(QContactOnlineAccount det, cons[0].details<QContactOnlineAccount>())
            {
                if (det.value("Account") == "999999@ovi.com" // deprecated, going to account URI
                        || det.accountUri() == "999999@ovi.com")
                {
                    QVERIFY(det.presence() == QContactOnlineAccount::PresenceOffline);
                    containDetail = true;
                }
            }
        QVERIFY(containDetail);
    }

    // remove them
    QVERIFY2(trackerEngine->removeContact(masterContactId, error), "Removing a contact failed");

    foreach(unsigned int id, idstomerge)
    {
        QVERIFY2(!trackerEngine->removeContact(id, error), "Merged contact doesn't exist and removing it shoudl fail");
    }
}

void ut_qtcontacts_trackerplugin::testIMContactsFilterring()
{
    QList<unsigned int> idstoremove;
    QList<QContactLocalId> idsToRetrieveThroughFilter;
    for( int i = 0; i < 3; i++ )
    {
        unsigned int contactid = qHash(QString("/org/freedesktop/fake/account/") + QString::number(999995+i) + "@ovi.com");
        idstoremove << contactid;
        insertContact(QString("telepathy:/org/freedesktop/fake/account/") + QString::number(999995+i) + "@ovi.com",
                contactid, QString::number(999995 + i)+ "@ovi.com", "nco:presence-status-available",
                QString("/org/freedesktop/fake/account/%1").arg(i/2), QString("ovi%1.com").arg(i/2));
        if(!i/2)
            idsToRetrieveThroughFilter << contactid;
    }

    {
    // now filter by service provider ovi0.com needs to return 2 contacts, 999995 & 999996
    QList<QContactLocalId> ids(idsToRetrieveThroughFilter);

    QContactFetchRequest request;
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactOnlineAccount::DefinitionName, QContactOnlineAccount::FieldServiceProvider);

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));
    filter.setValue(QString("ovi0.com"));
    filter.setMatchFlags(QContactFilter::MatchExactly);

    request.setDefinitionRestrictions(QStringList()<<QContactOnlineAccount::DefinitionName);
    request.setFilter(filter);

    trackerEngine->startRequest(&request);

    for(int i = 0; i < 100; i++)
    {
        usleep(100000);
        QCoreApplication::processEvents();
        if(request.isFinished() )
            break;
    }

    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QVERIFY(!request.contacts().isEmpty());

    QVERIFY(request.contacts().size() >= 2);
    foreach(const QContact &contact, request.contacts())
    {
        //qDebug() << contact.localId()<< "acc"<<contact.detail<QContactOnlineAccount>().serviceProvider();
        QVERIFY(contact.detail<QContactOnlineAccount>().serviceProvider() == "ovi0.com");
        ids.removeOne(contact.localId());
    }
    QVERIFY(ids.isEmpty());
    }

    // now account path filter
    {
    // now filter by account path 999995 & 999996
    QList<QContactLocalId> ids(idsToRetrieveThroughFilter);

    QContactFetchRequest request;
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactOnlineAccount::DefinitionName, "AccountPath");

    Slots slot;
    QObject::connect(&request, SIGNAL(resultsAvailable()),
            &slot, SLOT(resultsAvailable()));
    // see insertTpContact
    filter.setValue(QString("/org/freedesktop/fake/account/0"));
    filter.setMatchFlags(QContactFilter::MatchExactly);

    request.setDefinitionRestrictions(QStringList()<<QContactOnlineAccount::DefinitionName);
    request.setFilter(filter);

    trackerEngine->startRequest(&request);

    for(int i = 0; i < 100; i++)
    {
        usleep(100000);
        QCoreApplication::processEvents();
        if(request.isFinished() )
            break;
    }

    // if it takes more, then something is wrong
    QVERIFY(request.isFinished());
    QVERIFY(!request.contacts().isEmpty());

    QVERIFY(request.contacts().size() >= 2);
    foreach(const QContact &contact, request.contacts())
    {
        QVERIFY(contact.detail<QContactOnlineAccount>().serviceProvider() == "ovi0.com");
        ids.removeOne(contact.localId());
    }
    QVERIFY(ids.isEmpty());
    }


    // remove them
    foreach(unsigned int id, idstoremove)
    {
        QVERIFY2(trackerEngine->removeContact(id, error), "Removing a contact failed");
    }

}

void ut_qtcontacts_trackerplugin::testContactsWithoutMeContact() {
    QContact c;
    QContactName name;
    name.setFirstName("Totally");
    name.setLastName("Unique");
    c.saveDetail(&name);
    trackerEngine->saveContact(&c, error);
    QContactLocalId id = c.localId();  // Store ID for later removal. 
    
    // Prepare the filter for the request - we fetch only the one contact saved above.
    QList<QContactLocalId> ids;
    ids << id;
    QContactLocalIdFilter filter;
    filter.setIds(ids);
    
    // Prepare the requst - give filter to it and specify which fields to fetch. We fetch only the name.
    QStringList details;
    details << QContactName::DefinitionName;

    QContactLocalIdFetchRequest nameFetchRequest;
    nameFetchRequest.setFilter(filter);

    // Start the request and wait for it to finish.
    trackerEngine->startRequest(&nameFetchRequest);
    trackerEngine->waitForRequestFinished(&nameFetchRequest, 1000);

    // Requst finished. Test that only one contact is removed.
    QList<QContactLocalId> contacts = nameFetchRequest.ids();
    QVERIFY2(contacts.count() < 2, "We expected to get only one contact. Got more.");
    QVERIFY2(contacts.count() != 0, "We expected to get one contact. Got none.");
    QVERIFY2(contacts.first() == id, "Did not get the requested contact back.");
    
    // Cleaning up.
    trackerEngine->removeContact(id, error);

}

void ut_qtcontacts_trackerplugin::testDefinitionNames()
{
    QContactManager *cm(ContactManager::instance());
    QMap<QString, QContactDetailDefinition> defs(cm->detailDefinitions());

    foreach(QString key, defs.keys()) {
        QCOMPARE(defs[key].name(), key);
    }
}


/***************************     Helper functions for unit tests   ***************'*/

QContact ut_qtcontacts_trackerplugin::contact(QContactLocalId id, QStringList details)
{
    QContactManager::Error error;
    return trackerEngine->contact_impl(id, details, error);
}

QList<QContact> ut_qtcontacts_trackerplugin::contacts(QList<QContactLocalId> ids, QStringList details)
{
    QContactFetchRequest request;
    QContactLocalIdFilter filter;
    filter.setIds(ids);
    request.setFilter(filter);

    request.setDefinitionRestrictions(details);

    trackerEngine->startRequest(&request);
    trackerEngine->waitForRequestFinished(&request, 1000);

    return request.contacts();
}

void Slots::idResultsAvailable()
{
    QContactLocalIdFetchRequest* self = qobject_cast<QContactLocalIdFetchRequest*>(sender());
    ids << self->ids();
}

void Slots::resultsAvailable()
{
    QContactFetchRequest* self = qobject_cast<QContactFetchRequest*>(sender());
    contacts = self->contacts();
    QList<QContactLocalId> idsFromAllContactReq;
    foreach( QContact contact, contacts)
    {
        idsFromAllContactReq << contact.localId();
    }
}

QTEST_MAIN(ut_qtcontacts_trackerplugin)
