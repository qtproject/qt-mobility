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

#include "qtcontacts.h"
#include "requestexample.h"

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QTimer>

static void addContact(QContactManager*);
static void callContact(QContactManager*);
static void matchCall(QContactManager*, const QString&);
static void viewSpecificDetail(QContactManager*);
static void viewDetails(QContactManager*);
static void addPlugin(QContactManager*);
static void editView(QContactManager*);
static void loadManager();
static void loadManagerWithParameters();

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // manager configuration examples
    loadManager();
    loadManagerWithParameters();

    // synchronous API examples
    QContactManager* cm = new QContactManager();
    addContact(cm);
    callContact(cm);
    matchCall(cm, "111-222-333"); // unknown number.
    matchCall(cm, "12345678");    // alice's number.
    viewSpecificDetail(cm);
    viewDetails(cm);
    addPlugin(cm);
    editView(cm);

    // asynchronous API example
    RequestExample re;
    re.setManager(cm);
    QTimer::singleShot(10, &re, SLOT(performRequest()));
    app.exec();
    delete cm;

    return 0;
}

//! [Creating a new contact]
void addContact(QContactManager* cm)
{
    QContact alice;

    /* Set the contact's name */
    QContactName aliceName;
    aliceName.setFirst("Alice");
    aliceName.setLast("Jones");
    aliceName.setCustomLabel("Ally Jones");
    alice.saveDetail(&aliceName);

    /* Add a phone number */
    QContactPhoneNumber number;
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("12345678");
    alice.saveDetail(&number);
    alice.setPreferredDetail("DialAction", number);

    /* Add a second phone number */
    QContactPhoneNumber number2;
    number2.setContexts(QContactDetail::ContextWork);
    number2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    number2.setNumber("555-4444");
    alice.saveDetail(&number2);

    /* Save the contact */
    cm->saveContact(&alice);
}
//! [Creating a new contact]

//! [Calling an existing contact]
void callContact(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contacts();
    QContact a = cm->contact(contactIds.first());

    /* Get this contact's first phone number */
    QContactPhoneNumber phn = a.detail<QContactPhoneNumber>();
    if (!phn.isEmpty()) {
        // First, we need some way of retrieving the QObject which provides the action.
        // This may be through the (previously announced) Qt Service Framework:
        //QServiceManager* manager = new QServiceManager();
        //QObject* dialer = manager->loadInterface("com.nokia.qt.mobility.contacts.Dialer");
        //QContactAbstractAction* dialerImpl = static_cast<QContactAbstractAction*>dialer;
        //dialerImpl->performAction("DialActionId", a, phn);
    }
}
//! [Calling an existing contact]

//! [Filtering by definition and value]
void matchCall(QContactManager* cm, const QString& incomingCallNbr)
{
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(incomingCallNbr);
    phoneFilter.setMatchFlags(QContactFilter::MatchExactly);

    QList<QContactLocalId> matchingContacts = cm->contacts(phoneFilter);
    if (matchingContacts.size() == 0) {
        qDebug() << "Incoming call from unknown contact (" << incomingCallNbr << ")";
    } else {
        QContact match = cm->contact(matchingContacts.at(0));
        qDebug() << "Incoming call from"
                 << match.displayLabel()
                 << "(" << incomingCallNbr << ")";
    }
}
//! [Filtering by definition and value]

//! [Viewing a specific detail of a contact]
void viewSpecificDetail(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contacts();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "The first phone number of" << a.displayLabel()
             << "is" << a.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
}
//! [Viewing a specific detail of a contact]

//! [Viewing the details of a contact]
void viewDetails(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contacts();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Viewing the details of" << a.displayLabel();

    QList<QContactDetail> allDetails = a.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        QContactDetailDefinition currentDefinition = cm->detailDefinition(detail.definitionName());
        QMap<QString, QContactDetailDefinitionField> fields = currentDefinition.fields();

        qDebug("\tDetail #%d (%s):", i, detail.definitionName().toAscii().constData());
        foreach (const QString& fieldKey, fields.keys()) {
            qDebug() << "\t\t" << fieldKey << "(" << fields.value(fieldKey).dataType() << ") =" << detail.value(fieldKey);
        }
        qDebug();
    }
}
//! [Viewing the details of a contact]

//! [Installing a plugin which modifies a definition]
void addPlugin(QContactManager* cm)
{
    /* Find the definition that we are modifying */
    QMap<QString, QContactDetailDefinition> definitions = cm->detailDefinitions();
    QContactDetailDefinition modified = definitions.value(QContactEmailAddress::DefinitionName);

    /* Make our modifications: we add a "Label" field to email addresses */
    QContactDetailDefinitionField newField;
    newField.setDataType(QVariant::String);
    QMap<QString, QContactDetailDefinitionField> fields = modified.fields();
    fields.insert("Label", newField);

    /* Update the definition with the new field included */
    modified.setFields(fields);

    /* Save the definition back to the manager */
    if (cm->saveDetailDefinition(modified))
        qDebug() << "Successfully modified the detail definition!";
    else
        qDebug() << "This backend could not support our modifications!";
}
//! [Installing a plugin which modifies a definition]

//! [Modifying an existing contact]
void editView(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contacts();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Modifying the details of" << a.displayLabel();

    /* Change the first phone number */
    QList<QContactDetail> numbers = a.details(QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber phone = numbers.value(0);
    phone.setNumber("123-4445");

    /* Add an email address */
    QContactEmailAddress email;
    email.setEmailAddress("alice.jones@example");
    email.setContexts(QContactDetail::ContextHome);
    email.setValue("Label", "Alice's Work Email Address");

    /* Save the updated details to the contact. */
    a.saveDetail(&phone);
    a.saveDetail(&email);

    /* Now we must save the updated contact back to the database. */
    cm->saveContact(&a);
    viewDetails(cm);
}
//! [Modifying an existing contact]

//! [Asynchronous contact request]
void RequestExample::performRequest()
{
    // retrieve any contact whose first name is "Alice"
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    dfil.setValue("Alice");
    dfil.setMatchFlags(QContactFilter::MatchExactly);

    m_fetchRequest->setFilter(dfil);
    connect(m_fetchRequest, SIGNAL(progress(QContactFetchRequest*,bool)), this, SLOT(printContacts(QContactFetchRequest*,bool)));
    if (!m_fetchRequest->start()) {
        qDebug() << "Unable to request contacts!";
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested contacts; awaiting results...";
    }
}

void RequestExample::printContacts(QContactFetchRequest* request, bool appendOnly)
{
    QList<QContact> results = request->contacts();
    if (appendOnly) {
        // we know that the results are still in the same order; just display the new results.
        for (m_previousLastIndex += 1; m_previousLastIndex < results.size(); m_previousLastIndex++) {
            qDebug() << "Found another Alice:" << results.at(m_previousLastIndex).displayLabel();
        }
    } else {
        // the order of results has changed; display them all.
        for (m_previousLastIndex = 0; m_previousLastIndex < results.size(); m_previousLastIndex++) {
            qDebug() << "Found another Alice:" << results.at(m_previousLastIndex).displayLabel();
        }
    }

    // once we've finished retrieving results, stop processing events.
    if (request->state() == QContactAbstractRequest::FinishedState || request->state() == QContactAbstractRequest::CanceledState) {
        QCoreApplication::exit(0);
    }
}
//! [Asynchronous contact request]

//! [Loading a specific manager backend]
void loadManager()
{
    QContactManager* cm = new QContactManager("KABC");
    QList<QContactLocalId> contactIds = cm->contacts();
    if (!contactIds.isEmpty()) {
        QContact a = cm->contact(contactIds.first());
        qDebug() << "This manager contains" << a.displayLabel();
    } else {
        qDebug() << "This manager contains no contacts";
    }

    delete cm;
}
//! [Loading a specific manager backend]

//! [Loading a specific manager backend with parameters]
void loadManagerWithParameters()
{
    QMap<QString, QString> parameters;
    parameters.insert("Settings", "~/.qcontactmanager-kabc-settings.ini");
    QContactManager* cm = new QContactManager("KABC", parameters);
    QMap<QString, QContactDetailDefinition> definitions = cm->detailDefinitions();

    qDebug() << "This backend currently supports the following detail definitions:";
    QList<QContactDetailDefinition> allDefinitions = definitions.values();
    foreach (const QContactDetailDefinition& defn, allDefinitions) {
        QMap<QString, QContactDetailDefinitionField> fields = defn.fields();
        foreach (const QString& fieldKey, fields.keys()) {
            QList<QVariant> allowableValues = fields.value(fieldKey).allowableValues();
            qDebug() << "\t" << fieldKey << "(" << fields.value(fieldKey).dataType() << "):";
            if (allowableValues.isEmpty()) {
                qDebug() << "\t\tAny Value Permitted";
            } else {
                qDebug() << allowableValues;
            }
        }
    }

    delete cm;
}
//! [Loading a specific manager backend with parameters]
