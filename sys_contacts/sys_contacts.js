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

//TESTED_COMPONENT=src/contacts

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

// Returns true if contact named,name, can be found in contact list
function contactFind(name, partialMatch){
    var contactList = findWidget( { className: "QListWidget" } );
    var nameList = getValue(contactList).split("\n");
    var index = 0;
    var found = false;
    if (partialMatch == undefined){
        partialMatch = false;
    }
    for (index = 0; index < nameList.length; index++){
        if ((!partialMatch && nameList[index] == name)
            || (partialMatch && nameList[index].contains(name))){
            found = true;
            break;
        }
    }

    return found;
}


// returns true if automated testing can be done, false manual testing is to be done

function automatedTestEnvironment(){
      return (checkOS("symbian") || checkOS("maemo"));
      //return (checkOS("symbian") || checkOS("maemo") || checkOS("linux"));
}


// not ideal, there is need to programaticly check that 
//  the contact details dialog is not longer showing, ie no "Save" button
function waitForSave(){
    for (var loop=0; loop < 5; loop++){
        if (getLabels().contains("&Save"))
            wait(1000); // wait to try again
        else
            break;
    }
    
    // should not happen
    verify(!getLabels().contains("&Save"));
}

testcase = {


    initTestCase: function()
    {
        startApplication("samplephonebook");

        if (automatedTestEnvironment()){
            var contactList = findWidget( { className: "QListWidget" } );
            verify(getValue(contactList).length == 0, "There must be zero contacts prior to running test, there are the following contacts:" + getValue(contactList));
        }else{
            prompt(twiki('---+++ Initialize Contacts Test case
    1. Start the *samplephonebook* application on target and select a *Store:* value to suit !QtContacts Backend Engine for this test
                '));
        }
    },


    addContact_data : {
        Name1 : ["Name1", "Phone1", "Email1", "Address1", "testdata/avatar1.png"],
        Name2 : ["Name2", "Phone2", "Email2", "Address2", "testdata/avatar2.png"],
        Name3 : ["Name3", "Phone3", "Email3", "Address3", "testdata/avatar3.png"]
    },

    /* Requirement: Add some contacts */
    addContact: function(name, phone, email, address, avatar)
    {
        if (automatedTestEnvironment()){
            select("Add");
            enter(name, "Name");
            enter(phone, "Phone");
            enter(email, "Email");
            enter(address, "Address");
            var selectPictureBtn = findByProperty( { className: "QPushButton", text : "Set picture" });
            select(selectPictureBtn[0]);
            enter(avatar, "File name:");
            // need to accept entry due to autocompletion
            keyClick(Qt.Key_Enter);
            select("Save");
            waitForSave();
        }else{
            prompt(twiki('---+++ Verify Add Contact
    1. Click the *Add* button in *samplephonebook* application and in dialog that presents press the *Add Contact* button
    1. For all text fields shown in Contact Details dialog enter values in the format *!FieldName !ContactNumber*, for eg:
    | *Field Name* | *Value* |
    | Name    | ' + name    + ' |
    | Phone   | ' + phone   + ' |
    | Email   | ' + email   + ' |
    | Address | ' + address + ' |
    1. Click the *Set Picture* button
    1. In the file selection dialog that presents select file named ='  + avatar + '=
    1. If device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in additional contact(s).
    1. If device has additional avatars of different file formats and sizes then create a contact that makes use of a selection of those possible avatars
    1. Click the *Save* button
    1. *Verify* that the contact details dialog is closed
                '));
        }
    },


    checkAddedContactName_data : {
        Name1 : ["Name1"],
        Name2 : ["Name2"],
        Name3 : ["Name3"]
    },

    /* verify added contact by name  */
    checkAddedContactName: function(name)
    {
        if (automatedTestEnvironment()){
            waitForSave();
            var contactList = findWidget( { className: "QListWidget" } );
            mouseClick(contactList[0]);
            select(name);
        }else{
            prompt(twiki('---+++ Verify Added Contact
    1. *Verify* that the Contact List contains added contact, eg: =' + name +'=
                '));

        }

    },


    checkUpdateContactDetails_data : {
        Name1 : ["Name1", "Phone1", "Email1", "Address1", "testdata/avatar1.png"],
        Name2 : ["Name2", "Phone2", "Email2", "Address2", "testdata/avatar2.png"],
        Name3 : ["Name3", "Phone3", "Email3", "Address3", "testdata/avatar3.png"]
    },

    /* Requirement: Verify the contact details and confirm that it can be updated */
    /* verify added contact details and update contact name */
    checkUpdateContactDetails : function (name, phone, email, address, avatar)
    {
        if (automatedTestEnvironment()){
            waitForSave();
            var contactList = findWidget( { className: "QListWidget" } );
            mouseClick(contactList[0]);
            select(name);
            select("Edit");
            compare(getText("Name"), name);
            enter(name + ' 1', "Name");
            compare(getText("Phone"), phone);
            compare(getText("Email"), email);
            compare(getText("Address"), address);
            select("Save");
            waitForSave();
        }else{
            prompt(twiki('---+++ Verify View/Update Contact Details
    1. In *samplephonebook* application select contact *' + name + '* from Contact List
    1. Click the *Edit* button
    1. For all text fields shown in Contact Details dialog *Verify* they match the values entered ealier, eg:
    | *Field Name* | *Value* |
    | Name    | ' + name    + ' |
    | Phone   | ' + phone   + ' |
    | Email   | ' + email   + ' |
    | Address | ' + address + ' |
    1. *Verify* Avatar displayed looks like *'  + avatar + '*
    1. Click the *Save* button
                '));
        }
    },



    checkModifiedContactName_data : {
        Name1 : ["Name1 1"],
        Name2 : ["Name2 1"],
        Name3 : ["Name3 1"]
    },

    /* verify updated contact name*/
    checkModifiedContactName: function(name)
    {
        if (automatedTestEnvironment()){
            var contactList = findWidget( { className: "QListWidget" } );
            select(name, contactList[0]);
        }else{
            prompt(twiki('---+++ Verify a modified contact name
    1. *Verify* that contact *' + name + '* is in Contact List
                '));
        }
    },


    filterCheck_data : {
        Name1 : ["Name1 1", true, false,  "Full contact name match"],
        Name2 : ["2", true, true, "Partial contact name match"],
        Name3 : ["NonExistant", false, false, "Non-existant contact name"]

    },

    /* Requirement : Verify the correct match of contacts against a simple filter */
    /* Test the "Apply &Filter" function */
    filterCheck : function(name, exists, partialMatch, message) {
        if (automatedTestEnvironment()){
            if (checkOS("maemo")){
                skip("Maemo menus not supported yet", SkipAll);
                /* mouseClick(400,10); // raise menu manually
                var maemoMenuBar = findByProperty({ className : "QMaemo5ApplicationMenu" });
                print("Maemo menu = " + maemoMenuBar);
                print("filterCheck:" + activeWidgetInfo());
                */
            }else{
                print("filterCheck:" + activeWidgetInfo());
                var menuItemName = "Apply &Filter...";
                var menuItem = contactsMenuItem(menuItemName);
                verify(menuItem != undefined, "No such \"" + menuItemName + "\" menu");
                select(menuItem, menuBar());
                enter(name, "Name");
                select("&Filter");
                verify(contactFind(name, partialMatch) == exists, "Apply filter failed: " +name + ":" + exists + " " + message);
            }
        }else{
            if (exists){ 
                prompt(twiki('---+++ Verify Contact Filter
    1. Select menu *Apply Filter*
    1. The purpose of this test is to perform : ' + message + '
    1. Enter in the *Name* field the value *' + name +'*
    1. Click the *Filter* button
    1. *Verify* that the expected number of contacts are found 
                    '));
            }else{
                prompt(twiki('---+++ Verify Contact Filter
    1. Select menu *Apply Filter*
    1. Enter in the *Name* field the value *' + name + '* for a ' + message + '
    1. *Verify* that *zero* contacts are found that match current filter
                  '));
            }
        }
    },


    clearFilterCheck_data : {
        Name1 : ["Name1 1"]
    },

    /* Test the "Clear Filter" function, this is pre-requisite for deleteContact(name) */
    clearFilterCheck : function(name){
        if (automatedTestEnvironment()){
            if (checkOS("maemo")){
                skip("Maemo menus not supported yet", SkipAll);
            }else{
                select(contactsMenuItem("&Clear Filter"), menuBar());
                verify(contactFind(name), "Clear filter failed: Looking for " + name);
            }
        }else{
            prompt(twiki('---+++ Verify Clearing Contact filter
    1. Select menu *Clear Filter*
    1. *Verify* that contact named ' + name + ' is displayed
              '));
        }
    },


    deleteContact_data : {
        Name1 : ["Name3 1"],
        Name2 : ["Name1 1"],
        Name3 : ["Name2 1"]
    },

    /* Requirement: Verify that a contact can be deleted */
    deleteContact : function(name){
        if (automatedTestEnvironment()){
            var contactList = findWidget( { className: "QListWidget" } );
            mouseClick(contactList[0]);
            select(name);
            select("Delete");
            for (var loop=0; loop < 5;loop++){
                if (contactFind(name))
                    wait(1000); // wait to try again
                else
                    break;
            }
            verify(!contactFind(name), "Delete contact failed on " + name);
        } else {
            prompt(twiki('---+++ Verify remove Contact
    1. select contact ' + name + ' in Contact List
    1. click the *Delete* button.
    1. Verify that  ' + name + ' is no longer in Contact List
                '));

        }
  }


}
