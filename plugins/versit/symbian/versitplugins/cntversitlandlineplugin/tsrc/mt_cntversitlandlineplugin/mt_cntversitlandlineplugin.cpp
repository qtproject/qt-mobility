/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QPluginLoader>
#include <QStringList>
#include <QVersitReader>
#include <QVersitContactImporter>
#include <QVersitContactExporter>
#include <QVersitProperty>


#include <QContactName>

#include <QDebug>


#include "mt_cntversitlandlineplugin.h"



const char *CNT_VERSIT_PLUGIN_DIRECTORY = "/resource/qt/plugins/versit/";

QVersitContactHandlerFactory* MT_CntVersitLandlinePlugin::loadPlugin()
{
    QDir pluginsDir(CNT_VERSIT_PLUGIN_DIRECTORY);
     
    QVersitContactHandlerFactory* contactPlugin = NULL;
       
    if (pluginsDir.exists("cntversitlandlineplugin.qtplugin")) {
        QPluginLoader* pluginLoader = new QPluginLoader( 
                pluginsDir.absoluteFilePath("cntversitlandlineplugin.qtplugin"));
        if (pluginLoader->load()) {
            QObject *plugin = pluginLoader->instance();
            if (plugin) {
                contactPlugin = qobject_cast<QVersitContactHandlerFactory*>(plugin);  
            }
        }
    }
    
    return contactPlugin;
}



void MT_CntVersitLandlinePlugin::testPluginFactory() 
{
   
    QVersitContactHandlerFactory* contactPlugin = loadPlugin();
   
    QVERIFY(contactPlugin);
          
    QSet<QString> profiles = contactPlugin->profiles();
    QVERIFY(profiles.isEmpty());
                   
    QString name = contactPlugin->name();
    QVERIFY(name == QLatin1String("com.nokia.symbian.contacts.cntversitlandlineplugin"));
   
}

/*!
   Test detailProcessed() directly. 
   (detailProcessed is called during contact export for every detail)
   
   This test verifies that:
   
   1. the property matching the value for current detail being processed is
   found and updated from a list of output properties
   
   2. the property matching the value for current detail being processed is
   not updated if the property is not a telephone number
   
 */
void MT_CntVersitLandlinePlugin::testDetailProcessed() 
{
    // -----------------------------------------
    // 1. Load our plugin and create the handler
    // -----------------------------------------
    QVersitContactHandlerFactory* contactPlugin = loadPlugin();
    QVersitContactHandler* handler = NULL;
    
    if (contactPlugin)
        {
        handler = contactPlugin->createHandler();
        }
    
    // -----------------------------------------
    // 2. Create the test data
    // -----------------------------------------
    QContact contact;
    QVersitDocument document;
    QSet<QString> processedFields;
    QList<QVersitProperty> toBeRemoved;
       
    // the current detail (phone number) being exported
    QContactPhoneNumber number;
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    number.setNumber("12345678");
    
    QVersitProperty property1;   
    property1.setName(QLatin1String("TEL"));
    property1.setValue("35262222");
    property1.insertParameter(QLatin1String("TYPE"), QLatin1String("VOICE")); 
    property1.insertParameter(QLatin1String("TYPE"), QLatin1String("HOME")); 
 
    
    QVersitProperty property2;   
    property2.setName(QLatin1String("TEL"));
    property2.setValue(number.number());
    property2.insertParameter(QLatin1String("TYPE"), QLatin1String("HOME"));   
    
    
    QVersitProperty property3;   
    property3.setName(QLatin1String("TEL"));
    property3.setValue("34735634");
    property3.insertParameter(QLatin1String("TYPE"), QLatin1String("CELL"));
    property3.insertParameter(QLatin1String("TYPE"), QLatin1String("WORK"));   
    
    // evil test data, same value as number but it is an URL
    QVersitProperty property4;
    property4.setName(QLatin1String("URL"));
    property4.setValue(number.number());
          
    
    QList<QVersitProperty> toBeAdded;
    
    toBeAdded.append(property1);
    toBeAdded.append(property2);
    toBeAdded.append(property3);
    toBeAdded.append(property4);
    
    
    // -----------------------------------------
    // 3. Execute the test
    // -----------------------------------------
    handler->detailProcessed(contact, number, document,
                             &processedFields, &toBeRemoved, &toBeAdded);

    // -----------------------------------------
    // 4. Verify results
    // -----------------------------------------
    
    // T1. the property matching the value for current detail being processed is
    // found and updated from a list of output properties
    const QVersitProperty property2after = toBeAdded.at(1);
    QStringList typeParam = property2after.parameters().values(QLatin1String("TYPE"));
    QVERIFY(typeParam.contains(QLatin1String("VOICE")) );
    
    // T2. the property matching the value for current detail being processed is
    // not updated if the property is not a telephone number
    const QVersitProperty property4after = toBeAdded.at(3);     
    QStringList typeParam2 = property4after.parameters().values(QLatin1String("TYPE"));
    QVERIFY(!typeParam2.contains(QLatin1String("VOICE")) );
   
}





QVersitDocument MT_CntVersitLandlinePlugin::exportVCard(QContact contact )
{
    // get list of all versit-documents
    QVersitDocument::VersitType versitType(QVersitDocument::VCard21Type);
    QVersitContactExporter exporter;
    
    QList<QContact> contactList;
    contactList.append(contact);
                                               /**********************************/           
    bool ret_val = exporter.exportContacts(   /* <-- THIS WILL CALL OUR PLUGIN **/
        contactList, versitType);            /**********************************/
                
    QList<QtMobility::QVersitDocument> documentList = exporter.documents();    

    qDebug() << "versit document count:" << documentList.count();
    
    return documentList.first();
}

QContact MT_CntVersitLandlinePlugin::importVCard(QByteArray inputVCard)
{
    // Write VCard to input buffer
    QBuffer input;
    input.open(QBuffer::ReadWrite);
    input.write(inputVCard);
    input.seek(0);
       
    // Parse the input into QVersitDocument(s)
    QVersitReader reader;
    reader.setDevice(&input);
    reader.startReading();
    reader.waitForFinished();
       
    // Import contact from versit document
    // Calling importer.importDocuments will result to calls in our plugin
    QContact contact;
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();     
    if (versitDocuments.count() > 0 ) {      
                                                     /***********************************/  
        bool ret_val = importer.importDocuments(    /** <-- THIS WILL CALL OUR PLUGIN **/
                versitDocuments);                  /***********************************/
        
        QList<QContact> contacts = importer.contacts();
        if(contacts.count() > 0)
            contact = contacts.first();
    }
    
    return contact;
}


void  MT_CntVersitLandlinePlugin::testExportLandline()
{   
    QContact contact;
       
    /* Set the contact's name */
    QContactName contactName;
    contactName.setFirstName("Alice");
    contactName.setLastName("Jones");
    contactName.setCustomLabel("Ally Jones");
    contact.saveDetail(&contactName);
       
    /* Add a phone number */
    QContactPhoneNumber number;
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("12345678");
    contact.saveDetail(&number);
   
    /* Add a second phone number */
    QContactPhoneNumber number2;
    number2.setContexts(QContactDetail::ContextWork);
    number2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    number2.setNumber("555-4444");
    contact.saveDetail(&number2);
    
    /* Add a third phone number, just the same as above,
     * but is of SubTypeVoice */
    QContactPhoneNumber number3;
    number3.setContexts(QContactDetail::ContextWork);
    number3.setSubTypes(QContactPhoneNumber::SubTypeVoice);
    number3.setNumber("555-4444");
    contact.saveDetail(&number3);
    
    QVersitDocument result = exportVCard(contact);      
    qDebug() << result;
    
    QVERIFY(!result.isEmpty());
        
    QVersitProperty property;   
    property.setName(QLatin1String("TEL"));
    property.setValue(number.number());
    property.insertParameter(QLatin1String("TYPE"), QLatin1String("CELL"));
    property.insertParameter(QLatin1String("TYPE"), QLatin1String("HOME"));   
    qDebug() << property;
    QVERIFY(result.properties().contains(property)); 
  
    
    QVersitProperty property2;   
    property2.setName(QLatin1String("TEL"));
    property2.setValue(number2.number());
    property2.insertParameter(QLatin1String("TYPE"), QLatin1String("VOICE"));
    property2.insertParameter(QLatin1String("TYPE"), QLatin1String("WORK"));     
    qDebug() << property2;
    QVERIFY(result.properties().contains(property2)); 
    
    QVERIFY(result.properties().count(property2) == 2);
       
}
    
void  MT_CntVersitLandlinePlugin::testImportMobile()
{
    // Create the input vCard
    QByteArray inputVCard =
            "BEGIN:VCARD\r\n"
            "VERSION:2.1\r\n"
            "FN:John Citizen\r\n"
            "TEL;CELL:11111\r\n"           // mobile1
            "TEL;CELL;WORK:22222\r\n"      // mobile2
            "END:VCARD\r\n";    
    
    // Read the vCard
    QContact contact = importVCard(inputVCard);
    
    // Verify the results
    QList<QContactPhoneNumber> allnumbers = contact.details<QContactPhoneNumber>();
        
    // Check that expected count of telephone numbers was imported
    QVERIFY( allnumbers.count() == 2 );
        
    QContactPhoneNumber mobile1;
    mobile1.setNumber("11111");
    mobile1.setSubTypes(QContactPhoneNumber::SubTypeMobile);     
    QVERIFY(allnumbers.contains(mobile1));   
    
    QContactPhoneNumber mobile2;
    mobile2.setNumber("22222");
    mobile2.setSubTypes(QContactPhoneNumber::SubTypeMobile);   
    mobile2.setContexts(QContactDetail::ContextWork);
    QVERIFY(allnumbers.contains(mobile2)); 
}


void  MT_CntVersitLandlinePlugin::testImportLandline()
{
    // Create the input vCard
    QByteArray inputVCard =
            "BEGIN:VCARD\r\n"
            "VERSION:2.1\r\n"
            "FN:John Citizen\r\n"
            "TEL;VOICE;HOME:11111\r\n"
            "TEL;VOICE:22222\r\n"
            "END:VCARD\r\n";    
    
//    // Read the vCard
    QContact contact = importVCard(inputVCard);
       
    // Verify the results
    QList<QContactPhoneNumber> allnumbers = contact.details<QContactPhoneNumber>();
    
    QVERIFY( allnumbers.count() == 2 );
    
    foreach(QContactPhoneNumber num, allnumbers)
        {
        qDebug() << num;
        }
    
    QContactPhoneNumber landline1;
    landline1.setNumber("11111");
    landline1.setSubTypes(QContactPhoneNumber::SubTypeLandline);   
    landline1.setContexts(QContactDetail::ContextHome);
    QVERIFY(allnumbers.contains(landline1));   
    
    QContactPhoneNumber landline2;
    landline2.setNumber("22222");
    landline2.setSubTypes(QContactPhoneNumber::SubTypeLandline);    
    QVERIFY(allnumbers.contains(landline2));        
}


void  MT_CntVersitLandlinePlugin::testImportCombo()
{   
    // Create the input vCard
    QByteArray inputVCard =
            "BEGIN:VCARD\r\n"
            "VERSION:2.1\r\n"
            "FN:John Citizen\r\n"
            "TEL;CELL:11111\r\n"
            "TEL;VOICE;HOME:22222\r\n"
            "TEL;VOICE:33333\r\n"
            "TEL;CELL:44444\r\n"
            "TEL;VOICE;WORK:55555\r\n"
            "END:VCARD\r\n";      

    //    // Read the vCard
    QContact contact = importVCard( inputVCard );
    
    QList<QContactPhoneNumber> allnumbers = contact.details<QContactPhoneNumber>();
        
    QVERIFY( allnumbers.count() == 5 );
        
    QContactPhoneNumber mobile1;
    mobile1.setNumber("11111");
    mobile1.setSubTypes(QContactPhoneNumber::SubTypeMobile);   
    QVERIFY(allnumbers.contains(mobile1));      

    QContactPhoneNumber landline1;
    landline1.setNumber("22222");
    landline1.setSubTypes(QContactPhoneNumber::SubTypeLandline);   
    landline1.setContexts(QContactDetail::ContextHome);
    QVERIFY(allnumbers.contains(landline1));   
        
    QContactPhoneNumber landline2;
    landline2.setNumber("33333");
    landline2.setSubTypes(QContactPhoneNumber::SubTypeLandline);    
    QVERIFY(allnumbers.contains(landline2));     
        
    QContactPhoneNumber mobile2;
    mobile2.setNumber("44444");
    mobile2.setSubTypes(QContactPhoneNumber::SubTypeMobile);   
    QVERIFY(allnumbers.contains(mobile2));      

    QContactPhoneNumber landline3;
    landline3.setNumber("55555");
    landline3.setSubTypes(QContactPhoneNumber::SubTypeLandline);   
    landline3.setContexts(QContactDetail::ContextWork);
    QVERIFY(allnumbers.contains(landline3));      
}


QTEST_MAIN(MT_CntVersitLandlinePlugin)
