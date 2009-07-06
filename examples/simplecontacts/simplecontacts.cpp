/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtcontacts.h"
//#include "qcontactkabcbackenddetails.h"

#include <QDebug>
#include <QApplication>

static void addContact();
static void callContact();
static void matchCall();
static void viewDetails();
static void editView();
static void addPlugin();
static void serialize();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    addContact();
    callContact();
    matchCall();
    viewDetails();
    editView();
    addPlugin();
    serialize();
    return 0;
}

/* Sample code for adding a contact */
void addContact()
{
    QContactManager cm;
    QContact alice;

    /* Set the name */
    QContactName name;
    name.setFirst("Alice");
    name.setLast("Jones");
    qDebug() << "addContact():";
    qDebug() << "Contact display name:" << name.displayName();

    /* Set this separately (may not be supported on all platforms - corresponds to FN */
    name.setDisplayName("Ally Jones");
    qDebug() << "Contact display name:" << name.displayName();

    /* Set the name */
    alice.name() = name;

    /* Add a phone number */
    QContactPhoneNumber number;
    number.setAttribute("Context", "Home");
    number.setAttribute("SubType", "Mobile");
    number.setNumber("12345678");
    number.setValue("label", "Home Mobile");
    alice.saveDetail(&number);
    alice.setPreferredDetail("DialActionId", number);

    /* Add another one, too */
    QContactPhoneNumber number2;
    number2.setAttribute("Context", "Work");
    number2.setAttribute("SubType", "Landline");
    number2.setNumber("555-4444");
    number2.setValue("label", "Work Landline");
    alice.saveDetail(&number2);

    /* Save it */
    cm.saveContact(&alice);
    qDebug() << "Just saved Alice as contact with id =" << alice.id();
}

/* Sample code for calling a contact */
void callContact()
{
    QContactManager cm;
    QContact a = cm.contact(cm.contacts().value(0)); // Does assume there's at least one contact...

    /* Get all the phone properties */
    QList<QContactDetail> numbers = a.details("PhoneNumber");

    if (numbers.count() > 1) {
        /* Prompt the user for a specific one */
        qDebug() << "Contact" << a.name().displayName() << "has " << numbers.count() << " numbers:";
        foreach (const QContactPhoneNumber& number, numbers) {
            qDebug() << number.value("label") << ": " << number.number();
        }
    } else if (numbers.count() == 1) {
        /* Just one */
        qDebug() << "Contact" << a.name().displayName() << "has only one number:" << numbers[0].value("label") << ": " << ((QContactPhoneNumber)numbers[0]).number();
    } else {
        /* Cannot call this contact */
        qDebug() << "Contact" << a.name().displayName() << "has no telephone numbers.";
    }
}

/* Sample code for matching a call */
void matchCall()
{
    // XXX TODO
}

/* Sample code for viewing the details of a contact */
void viewDetails()
{
    QContactManager cm;
    QContact a = cm.contact(cm.contacts().value(0)); // Does assume there's at least one contact...
    qDebug() << "viewDetails():";
    qDebug() << "Retrieved contact with id =" << a.id();

    /* We want to display the name first */
    QContactName name = a.name();

    /* Do display stuff with name */
    qDebug() << QString("Contact '%1':").arg(name.displayName()).toLatin1();
    qDebug() << QString("First name: %1").arg(name.first()).toLatin1();
    qDebug() << QString("Middle name: %1").arg(name.middle()).toLatin1();
    qDebug() << QString("Last name: %1").arg(name.last()).toLatin1();

    /* Now phone numbers */
    QList<QContactDetail> numbers = a.details("PhoneNumber");
    foreach(const QContactPhoneNumber& detail, numbers) {
        QString preferred = a.isPreferredDetail("DialActionId", detail) ? "preferred" : "";
        qDebug() << QString("%1 %2 phone %5: '%3' ('%4')").arg(detail.attributes().value("Context")).arg(detail.attributes().value("SubType")).arg(detail.value("label")).arg(detail.number()).arg(preferred);
    }

    /* etc.. now physical addresses */
    //QList<QContactDetail> addresses  = a.details("StreetAddress");
    //foreach(const QContactAddress& detail, addresses) {
    //    QString preferred = a.isPreferredDetail("DialActionId", detail) ? "preferred" : "";
    //    qDebug() << QString("%1 address %4: '%2' ('%3')").arg(detail.attributes().value("Context")).arg(detail.value("label")).arg(detail.displayLabel()).arg(preferred);
    //}

    /* Now email addresses etc.... */

}

/* Sample code for editing a contact */
void editView()
{
    /* This relies on addContact :) */
    QContactManager cm;
    QContact a = cm.contact(cm.contacts().value(0)); // Does assume there's at least one contact...
    qDebug() << "editView():";
    qDebug() << "Retrieved contact with id =" << a.id();

    /* We want to display the name first */
    QContactName name = a.name();
    qDebug() << "Editing" << name.displayName();

    QList<QContactDetail> numbers = a.details("PhoneNumber");

    /* Change the home phone number */
    QContactPhoneNumber detail = numbers.value(0);
    detail.setNumber("123-4445");
    a.saveDetail(&detail);

    /* and make the bus phone preferred */
    a.setPreferredDetail("DialActionId", numbers.value(1));

    /* add another number */
    QContactPhoneNumber newDetail;
    newDetail.setNumber("111-2222-333");
    newDetail.setAttribute("Context", "Work");
    newDetail.setAttribute("Subtype", "Landline");
    newDetail.setValue("label", "Second work number");
    a.saveDetail(&newDetail);

    /* save it */
    cm.saveContact(&a);
    qDebug() << "Saved that contact with id =" << a.id();

    qDebug() << "Finished editing - new details:";
    viewDetails();
}

/* Sample code showing how a plugin can add a property and store it to some contacts */
void addPlugin()
{
    /* TODO */
}

/* Sample code showing how a contact may be serialized to vcard */
void serialize()
{
    /* Serializer missing from this branch */
#if 0
    QContact alice;

    /* Set the name */
    QContactName name;
    name.setFirst("Alice");
    name.setLast("Jones");
    qDebug() << "Contact display name:" << name.displayName();

    /* Set this separately (may not be supported on all platforms - corresponds to FN */
    name.setDisplayName("Ally Jones");
    qDebug() << "Contact display name:" << name.displayName();

    /* Set the name */
    alice.name() = name;

    /* Add a phone number */
    QContactPhoneNumber number;
    number.setContext("Home");
    number.setAttribute("Mobile");
    number.setNumber("12345678");
    number.setValue("label", "Home Mobile");
    alice.addProperty(number);
    alice.setPreferredProperty("DialActionId", number);

    /* Add another one, too */
    number.setContext("Work");
    number.setAttribute("Landline");
    number.setNumber("555-4444");
    number.setLabel("Work Landline");
    alice.addProperty(number);

    /* serialize the contact to vcard */
    qDebug() << "--------------------------------------";
    QStringList serializedContact = QContactSerializer::serialize(alice);
    foreach (QString vcardLine, serializedContact)
        qDebug("%s", vcardLine.toAscii().data());

    /* deserialize that vcard back to a contact */
    QContact deserializedContact = QContactSerializer::deserialize(serializedContact);

    /* then serialize _that_ contact, print it out, and check for inconsistencies. */
    qDebug() << "--------------------------------------";
    QStringList reserializedContact = QContactSerializer::serialize(deserializedContact);
    foreach (QString vcardLine, reserializedContact)
        qDebug("%s", vcardLine.toAscii().data());

    qDebug() << "--------------------------------------";
    qDebug() << "Serialization and Deserialization" << (serializedContact == reserializedContact ? "is" : "is not") << "Symmetric!";
#endif
}
