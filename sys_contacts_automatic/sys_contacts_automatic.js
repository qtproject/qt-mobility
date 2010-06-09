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

testcase = {


  initTestCase: function()
  {
      startApplication("samplephonebook");
  },


  /* Requirement: Add some contacts */
  addContact: function(name, phone, email, address, avatar)
  {
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
    select("&Save");
  },

  addContact_data : {
    Name1 : ["Name1", "Phone1", "Email1", "Address1", "testdata/avatar1.png"],
    Name2 : ["Name2", "Phone2", "Email2", "Address2", "testdata/avatar2.png"],
    Name3 : ["Name3", "Phone3", "Email3", "Address3", "testdata/avatar3.png"]
  },


  /* verify added contact by name  */
  checkAddedContactName: function(name)
  {
    var contactList = findWidget( { className: "QListWidget" } );
    select(name, contactList[0]);
  },

  checkAddedContactName_data : {
    Name1 : ["Name1"],
    Name2 : ["Name2"],
    Name3 : ["Name3"]
  },


  /* Requirement: Verify the contact details and confirm that it can be updated */
  /* verify added contact details and update contact name */
  checkContactDetails : function (name, phone, email, address)
  {
    var contactList = findWidget( { className: "QListWidget" } );
    select(name, contactList[0]);
    select("&Edit");
    compare(getText("Name"), name);
    enter(name+"*", "Name");
    compare(getText("Phone"), phone);
    compare(getText("Email"), email);
    compare(getText("Address"), address);
    select("&Save");
  },

  checkContactDetails_data : {
    Name1 : ["Name1", "Phone1", "Email1", "Address1"],
    Name2 : ["Name2", "Phone2", "Email2", "Address2"],
    Name3 : ["Name3", "Phone3", "Email3", "Address3"]
  },


  /* verify updated contact name*/
  checkModifiedContactName: function(name)
  {
    var contactList = findWidget( { className: "QListWidget" } );
    select(name, contactList[0]);
  },
  checkModifiedContactName_data : {
    Name1 : ["Name1*"],
    Name2 : ["Name2*"],
    Name3 : ["Name3*"]
  },


  /* Requirement : Verify the correct match of contacts against a simple filter */
  /* Test the "Apply &Filter" function */
  filterCheck : function(name, exists, message, partialMatch) {
    var menuItemName = "Apply &Filter...";
    var menuItem = contactsMenuItem(menuItemName);
    verify(menuItem != undefined, "No such \"" + menuItemName + "\" menu") 
    select(menuItem, menuBar());
    enter(name, "Name");
    select("&Filter");
    verify(contactFind(name, partialMatch) == exists, "Apply filter failed: " +name + ":" + exists + " " + message);
  },

  filterCheck_data : {
    Name1 : ["Name1*", true, false,  "Full contact name match"],
    Name2 : ["2", true, true, "Partial contact name match"],
    Name3 : ["NonExistant", false, false, "Non-existant contact name"]

  },


  /* Test the "Clear Filter" function, this is pre-requisite for deleteContact(name) */
  clearFilterCheck : function(name){
    select(contactsMenuItem("&Clear Filter"), menuBar());
    verify(contactFind(name), "Clear filter failed: Looking for " + name);
  },

  clearFilterCheck_data : {
    Name1 : ["Name1*"]
  },


  /* Requirement : Verify contact export*/
  /* If versit suport is present test it */
  exportContacts: function(name1, name2, name3)
  {
    var menuItemName = "Ex&port contacts...";
    var menuItem = contactsMenuItem(menuItemName);
    if (menuItem == undefined)
        skip("Versit support not enabled", SkipAll);

    select(menuItem, menuBar());

    var contactsVCFFile = "testdata/contacts.vcf";

    enter(contactsVCFFile, "File name:");
    select("&Save");
    var contactVCFData = getData(contactsVCFFile);
    if (!(contactVCFData.indexOf(name1)
        && contactVCFData.indexOf(name2)
        && contactVCFData.indexOf(name3))){
        print(".vcf contains:" + contactVCFData);
        deletePath(contactsVCFFile);
        fail("Did not find in .vcf:" + name1 + "," + name2 + "," + name3);
    }
    deletePath(contactsVCFFile);

  },

  exportContacts_data : {
    NameSet1 : ["FN:Name1*", "FN:Name2*", "FN:Name3*"]
  },
    

  /* Requirement: Verify that a contact can be deleted */
  deleteContact : function(name){
    var contactList = findWidget( { className: "QListWidget" } );
    select(name, contactList[0]);
    select("&Delete");

    verify(!contactFind(name), "Delete contact failed on " + name);
  },
  deleteContact_data : {
    Name1 : ["Name3*"],
    Name2 : ["Name1*"],
    Name3 : ["Name2*"]
  },


}
