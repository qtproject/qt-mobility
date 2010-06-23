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

    add_a_contact: function() {

        prompt(twiki('---+++ Verify a contact can be added
    1. start the =samplephonebook= application and select the !QtContacts Backend Engine
    1. click the =Add= button and in dialog that presents press the =Add Contact= button
    1. for all text fields shown in Contact Details dialog enter values in the format *ContactNumber !FieldName*, for example
    | *Field Name* | *Value* |
    | Name | 1 Name |
    | Phone | 1 Phone |
    | Email | 1 Email |
    | Address | 1 Address |
    1. click the filled rect button  next to Avatar field and in the file selection dialog that presents select avatar1.png
    1. click the Save button
    1. repeat steps 1 to 4 for other two more contacts
    1. if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in additional contact(s).
    1. if device has additional avatars of different file formats and sizes then create a contact that makes use of a selection of those possible avatars
     '));
    },

    update_an_existing_contact: function() {
        prompt(twiki('---+++ Verify a contact can be updated
    1. select a contact in list, eg 1 Name via a mouse click
    1. click the Edit button
    1. verify the that details entered in step 2. 3. are presented and correct.
    1. change the Contact name, eg append * to end of name
    1. click the Save button
    1. verify that the new contact name is shown in list
    1. repeat steps 1. to 6 for each contact in list
        '));
    },

    delete_a_contact: function() {
        prompt(twiki('---+++ Verify a contact can be deleted
    1. select a contact in list at random, via a mouse click
    1. click the "Delete" button
    1. verify that the selected contact is no longer in contact list
        '));
    },

    add_and_edit_a_contact_group: function()
    {

        skip("This test is not currently possible, development may re-add the code to support this test later on");

        prompt(twiki('---+++ Verify a contact group can be added, updated and deleted
    1. start the =samplephonebook= application and select the !QtContacts Backend Engine to use
    Add some Contact Groups
    1. click the "Add" button and in Dialog that presents press the "Add Group" button
    1. in Group Details dialog enter value for Group name in the format =GroupNumber Group=, for example
    * Group Name has value *1 Group*
    1. in Group Details dialog click the "Add" button
    1. repeat steps 1. to 4. for two more groups
    1. if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in an additional Group Name.
    1. click the "Done"  button


    Add some contacts to use created Contact Groups
    1. click the "Add" button and in the dialog that is presented press the "Add Contact" button
    1. in Contact Details dialog enter values for Contact name in the format *ContactNumber !FieldName*, for example
    * Name has value =1 Name=
    1. click "Group Details" button
    1. in Contact Details dialog click the check box for "1 Group"
    1. click the "Ok" button to accept Group choice(s)
    1. click the "Save" button in Contact Details dialog
    1. repeat step 1.i to 6.i for three more contacts choosing different group combinations for each contact

    Verify that the selected Contact Groups per contact
    1. select a contact from list via a mouse click
    1. for a device smaller than 481 pixels  click the "Open" button
    1. click "Group Details" button verify that the contact has the groups chosen in step 10
    1. click "Cancel" button
    1. Repeat steps 1.  to 3.i for each contact

    Verify that a Contact Group can be updated
    1. click "Add button" and in dialog that is presented press the "Add Group" button
    1. in the Group Details dialog select a Group Name with a Mouse Click
    1. change the value for Group name, append a "*"
    1. click the "Save" button
    1. confirm that the entered Group Name has been updated in list
    1. Repeat steps 2. to 5. for each Group
    1. click the "Done" button

    Verify that a Contact Group can be deleted
    1. click the "Add button" and in dialog that is presented press the "Add Group" button
    1. in Group Details select a Group Name with a Mouse Click at random
    1. click the "Delete" button
    1. confirm that the Group Name has been deleted from in list
    1. repeat steps 2.vi) to 4.vi) for each Group
    1. click the "Done" button
        '));
    },

    filtering_a_contact_by_name: function()
    {
        skip("This test is not currently possible, development may re-add the code to support this test later on.");

        prompt(twiki('---+++ Verify the correct match of contacts against a simple filter
    If no contacts are present then add some contacts:
    1. click the "Add button" and in dialog that is presented press the "Add Contact" button
    1. in Contact Details dialog enter a value for the Contact name and number fields
    1. repeat for a at least three contacts
    1. if device has non-Latin1 keyboard input then create a Contact that makes use of a selection of such keys in additional contact(s).

    Find a existing contact:
    1. decide on which contact to look for from the contact shown
    1. click the "Find" button
    1. enter the complete name for contact in "Contact name" field
    1. click the "Find" button
    1. confirm that the contact was found as expected ( contact will be selected in list)
        '));
    },

    filtering_to_no_results: function()
    {
        skip("This test is not currently possible, development may re-add the code to support this test later on.");
        prompt(twiki('---+++ Verify that an empty filtering result set behaves correctly
    1. click the "Find" button
    1. enter a complete name for contact that does not exist on the device in "Contact name" field
    1. click the "Find" button
    1. confirm that the contact was not found as expected, an error dialog will be shown
        '));
    },

    complex_filter: function()
    {
        skip("This test is not currently possible, development may re-add the code to support this test later on.");
        prompt(twiki('---+++ Verify the correct match of contacts against a complex filter
    Find a existing contact:
    1. decide on which contact to look for from the contact shown
    1. click the "Find" button
    1. enter the complete name for contact in the "Search String" field
    1. click the "Add" button
    1. click the "Filter"  button
    1. confirm that the expected contact name is shown

    Search for non-existant contact:
    1. decide on name that is not listed in contact list shown, eg *Tester*
    1. click the "Find" button
    1. enter the complete name for contact in the "Search String" field
    1. click the "Add" button
    1. click the "Filter"  button
    1. confirm that the contact was not found, ie no names shown in contact list
        '));
    }
}
