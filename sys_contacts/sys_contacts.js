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


testcase = {

  contacts1: function()
  {
      prompt(twiki('---+++ Verify a contact can be added, updated and deleted

1.i) start the =samplephonebook= application and select the !QtContacts Backend Engine

Add some contacts:
   1.ii) click the "Add" button and in dialog that presents press the "Add Contact" button
   1.ii) for all text fields shown in Contact Details dialog enter values in the format *ContactNumber !FieldName*, for example
      | *Field Name* | *Value* |
      | Name | 1 Name |
      | Phone | 1 Phone |
      | Email | 1 Email |
      | Address | 1 Address |
   1.ii) click the "filled rect (button)"  next to "Avatar" field and in the file selection dialog that presents select "avatar1.png"
   1.ii) click the "Save" button
   1.ii) repeat steps 1.ii) to 4.ii) for other two more contacts
   1.ii) if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in additional contact(s).
   1.ii) if device has additional avatars of different file formats and sizes then create a contact that makes use of a selection of those possible avatars 

Verify the contact details and confirm that it can be updated:
   1.iii) select a contact in list, eg "1 Name" via a mouse click
   1.iii) click the "Edit" button
   1.iii) verify the that details entered in step 2.ii) 3.ii) are presented and correct.
   1.iii) change the Contact name, eg append "*" to end of name
   1.iii) click the "Save" button
   1.iii) verify that the new contact name is shown in list 
   1.iii) repeat steps 1.iii) to 6.iii) for each contact in list 

Verify that a contact can be deleted:
   1.iv) select a contact in list at random, via a mouse click
   1.iv) click the "Delete" button
   1.iv) verify that the selected contact is no longer in contact list
   1.iv) repeat step 1.iv) to 3.iv) until all contacts are deleted '));
  },

  contacts2: function()
  {
      prompt(twiki('---+++ Verify a contact group can be added, updated and deleted

*This test is not currently possible, development may re-add the code to support this test later on.*

1.i) start the =samplephonebook= application and select the !QtContacts Backend Engine to use

Add some Contact Groups
   1.ii) click the "Add" button and in Dialog that presents press the "Add Group" button
   1.ii) in Group Details dialog enter value for Group name in the format =GroupNumber Group=, for example
      * Group Name has value *1 Group*
   1.ii) in Group Details dialog click the "Add" button
   1.ii) repeat steps 1.ii) to 4.ii) for two more groups
   1.ii) if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in an additional Group Name. 
   1.ii) click the "Done"  button


Add some contacts to use created Contact Groups
   1.iii) click the "Add" button and in the dialog that is presented press the "Add Contact" button
   1.iii) in Contact Details dialog enter values for Contact name in the format *ContactNumber !FieldName*, for example
      * Name has value =1 Name=
   1.iii) click "Group Details" button 
   1.iii) in Contact Details dialog click the check box for "1 Group"
   1.iii) click the "Ok" button to accept Group choice(s)
   1.iii) click the "Save" button in Contact Details dialog
   1.iii) repeat step 1.iii) to 6.iii) for three more contacts choosing different group combinations for each contact

Verify that the selected Contact Groups per contact
   1.iv) select a contact from list via a mouse click
   1.iv) for a device smaller than 481 pixels  click the "Open" button
   1.iv) click "Group Details" button verify that the contact has the groups chosen in step 10
   1.iv) click "Cancel" button
   1.iv) Repeat steps 1.iv) to 3.iv) for each contact  

Verify that a Contact Group can be updated
   1.v) click "Add button" and in dialog that is presented press the "Add Group" button
   1.v) in the Group Details dialog select a Group Name with a Mouse Click
   1.v) change the value for Group name, append a "*"
   1.v) click the "Save" button
   1.v) confirm that the entered Group Name has been updated in list 
   1.v) Repeat steps 2.v) to 5.v) for each Group
   1.v) click the "Done" button 

Verify that a Contact Group can be deleted
   1.vi) click the "Add button" and in dialog that is presented press the "Add Group" button
   1.vi) in Group Details select a Group Name with a Mouse Click at random
   1.vi) click the "Delete" button
   1.vi) confirm that the Group Name has been deleted from in list 
   1.vi) repeat steps 2.vi) to 4.vi) for each Group
   1.vi) cick the "Done" button 
'));
  },

  contacts3: function()
  {
      prompt(twiki('---+++ Verify the correct match of contacts against a simple filter

*This test is not currently possible, development may re-add the code to support this test later on.*

If no contacts are present then add some contacts:
   1.i) click the "Add button" and in dialog that is presented press the "Add Contact" button
   1.i) in Contact Details dialog enter values for Contact name in the format =ContactNumber !FieldName=, for example
      * Name has value =1 Name=
   1.i) repeat steps 1.i) ,2.i) for a at least three contacts
   1.i) if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in additional contact(s).

Find a existing contact:
   1.ii) decide on which contact to look for from the contact shown
   1.ii) click the "Find" button
   1.ii) enter the complete name for contact in "Contact name" field
   1.ii) click the "Find" button
   1.ii) confirm that the contact was found as expected ( contact will be selected in list)

Search for non-existant contact:
   1.ii) decide on name that is not listed in contact list shown, eg *Tester*
   1.ii) click the "Find" button
   1.ii) enter the complete name for contact in "Contact name" field
   1.ii) click the "Find" button
   1.ii) confirm that the contact was not found as expected, an error dialog will be shown
'));
  },

  contacts4: function()
  {
      prompt(twiki('---+++ Verify the correct match of contacts against a complex filter 

    1.i) If no contacts are present then follow steps in previous section:
        "If no contacts are present then add some contacts:" 

Find a existing contact:
   1.ii) decide on which contact to look for from the contact shown
   1.ii) click the "Find" button
   1.ii) enter the complete name for contact in the "Search String" field
   1.ii) click the "Add" button
   1.ii) click the "Filter"  button
   1.ii) confirm that the expected contact name is shown

Search for non-existant contact:
   1.iii) decide on name that is not listed in contact list shown, eg *Tester*
   1.iii) click the "Find" button
   1.iii) enter the complete name for contact in the "Search String" field
   1.ii) click the "Add" button
   1.iii) click the "Filter"  button
   1.iii) confirm that the contact was not found, ie no names shown in contact list
'));
  }
}