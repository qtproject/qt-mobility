/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of QtUiTest.
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

//TESTED_COMPONENT=src/versit

// helper function re :contact actions may be in a separate "Contacts menu"
function contactsMenuItem(menuText)
{
    var menuEntries = getValue(menuBar()).toString().split("\n");
    var index = 0;
    var separateMenu = false; // optimize number of calls to indexOf

    for (index = 0; index < menuEntries.length; index++){
        if (!separateMenu && menuEntries[index].indexOf("&Contacts") >= 0){
            separateMenu = true;
            menuText = "&Contacts/" + menuText;
        }
        if (menuEntries[index] == menuText)
            return menuText;
    }

    return undefined;

}


// returns true if automated testing can be done, false manual testing is to be done
function automatedTestEnvironment(){
      return (checkOS("symbian") || checkOS("maemo"));
      //return (checkOS("symbian") || checkOS("maemo") || checkOS("linux"));
}

function importContactsMenu() {
    return contactsMenuItem("&Import contacts...");
}

function exportContactsMenu() {
    return contactsMenuItem("Ex&port contacts...");
}

testcase = {


    initTestCase: function() {
        startApplication("samplephonebook");

        if (!automatedTestEnvironment()){
            prompt(twiki('---+++ Initialize Contacts Test case
    1. Start the =samplephonebook= application on target and select a *Store:* value to suit !QtContacts Backend Engine for this test
    1. If =Import Contacts...= menu and =Export Contacts...= menu is missing skip all tests
                '));
        }

    },


    importContacts_data : {
        vCard1 : ["testdata/testimport1.vcf"]
    },

    importContacts: function(vCardFile) {
        if (automatedTestEnvironment()){
            var menuItem = importContactsMenu();
            if (menuItem == undefined)
                skip("Versit support not enabled", SkipAll);

            select(menuItem, menuBar());


            enter(vCardFile, "File name:");
            // need to accept entry due to autocompletion
            keyClick(Qt.Key_Enter);
        } else {
            prompt(twiki('---+++ Verify Can Import Contacts
    1. If =Import Contacts...= menu is missing skip this test
    1. Select the =Import Contacts...= menu
    1. Enter the vCard file name of : *' + vCardFile + '*
    1. Accept the value for vCard file (close file selection dialog)
    1. Verify that no errors are present on screen
                '));
        }

    },



    checkImportedContact_data : {
        Name1 : ["John Smith", "+123456", "John.Smith@SmithsComputerServices.com", "1/21 Anywhere St"],
        Name2 : ["Bill Smith", "+9876", "Bill.Smith@SmithsComputerServices.com", "2/21 Anywhere St"]
    },

    /* Requirement: Verify the contact details */
    checkImportedContact : function (name, phone, email, address) {
        if (automatedTestEnvironment()){
            if (importContactsMenu() == undefined)
                skip("Versit support not enabled", SkipAll);

            var contactList = findWidget( { className: "QListWidget" } );
            select(name, contactList[0]);
            select("&Edit");
            compare(getText("Name"), name);
            compare(getText("Phone"), phone);
            compare(getText("Email"), email);
            compare(getText("Address"), address);
            select("Cancel");
        }else{
            prompt(twiki('---+++ Verify Imported Contacts
    1. If =Import Contacts...= menu is not present skip this test
    1. Select contact *' + name + '* from Contact List
    1. Click the =Edit= button
    1. For all text fields shown in Contact Details dialog *Verify* they match the values imported, eg:
    | *Field Name* | *Value* |
    | Name    | ' + name    + ' |
    | Phone   | ' + phone   + ' |
    | Email   | ' + email   + ' |
    | Address | ' + address + ' |
    1. Verify that the Avatar looks like the word *BAT*
    1. Click the =Cancel= button
                '));
        }
    },


    exportContacts_data : {
        NameSet1 : ["testdata/contacts.vcf", "FN:John Smith", "FN:Bill Smith"]
    },

    /* Requirement : Verify contact export*/
    /* If versit suport is present test it */
    exportContacts: function(vCardFile, name1, name2)
    {
        if (automatedTestEnvironment()){
            var menuItem = exportContactsMenu();
            if (menuItem == undefined)
                skip("Versit support not enabled", SkipAll);

            select(menuItem, menuBar());

            enter(vCardFile, "File name:");
            // need to accept entry due to autocompletion
            keyClick(Qt.Key_Enter);
            var contactVCFData = getData(vCardFile);
            if (!(contactVCFData.indexOf(name1)
                && contactVCFData.indexOf(name2))) {
                print(".vcf contains:" + contactVCFData);
                deletePath(contactsVCFFile);
                fail("Did not find in .vcf:" + name1 + "," + name2);
            }
            deletePath(vCardFile);
        }else{
            prompt(twiki('---+++ Verify Exported Contacts
    1. If =Export Contacts...= menu is missing skip this test
    1. Select the =Export Contacts...= menu
    1. Enter name of contacts vCard as *' + vCardFile + '*
    1. Verify that *' + vCardFile + '* contains *' + name1 + '* and *' + name2 + '*
                '));
        }

    }

}
