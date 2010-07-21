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

//TESTED_COMPONENT=src/messaging

// returns true if automated testing can be done, false manual testing is to be done
function automatedTestEnvironment(){
      //Automated testing not supported for this test case yet

      return false;

      //return (checkOS("symbian") || checkOS("maemo"));
      //return (checkOS("symbian") || checkOS("maemo") || checkOS("linux"));
}

testcase = {

    serviceAction_sendplainTextMessage: function (){
        if (automatedTestEnvironment()){
        }else{
        prompt(twiki('---++++ ServiceAction: Send Message in Plain Text
    1. Launch *serviceeaction* application
    1. Fill in To, Subject, Content
    1. Click "Add attachment" menu item and select file(s) to attach
    1. Click on "Send" menu entry
    1. Check if the mail has been sent to the desired mailbox with the correct subject, body and complete attachment(s) 
        '));
        }
    },

    serviceAction_sendHTMLMessage : function() {
        if (automatedTestEnvironment()){
        }else{
            prompt(twiki('---++++ ServiceAction: Send Message in HTML
    1. Launch *serviceaction* application
    1. Fill in To, Subject, Content
    1. Click "Add attachment" menu item and select file(s) to attach
    1. Click on "Send as HTML" menu entry
    1. Check if the mail has been sent to the desired mailbox with the correct subject, body and complete attachment(s) 
        '));
        }
    },


    serviceAction_composeAndSend : function() {
        if (automatedTestEnvironment()){
        }else{
            prompt(twiki('---++++ ServiceAction: Compose and Send 
    1. Launch *serviceaction* application
    1. Fill in To, Subject, Content
    1. Click "Add attachment" menu item and select file(s) to attach
    1. Click on "Compose" menu entry
    1. Confirm that the Outlook mail client for Windows is now showing
    1. Click on the "Send" menu button in Outlook client
    1. Check if the mail has been sent to the desired mailbox with the correct subject, body and complete attachment(s) 
        '));
        }
    },

    write_message : function() {
        if (automatedTestEnvironment()){
        }else{
            prompt(twiki('---++++ Write Message 
    1. Launch *writemessage* application
    1. Fill in To, Subject, Content
    1. Click "Add attachment" menu item and select file(s) to attach
    1. Click on "Send" menu entry
    1. Check if the mail has been sent to the desired mailbox with the correct subject, body and complete attachment(s) 
        '));
        }
    },


    query_messages : function() {
        if (automatedTestEnvironment()){
        }else{
            prompt(twiki('---++++ Query Messages
    For WinCE start the example in Visual Studio since query messages only produces output to the console.

    1. Send an email to the test account
    1. Launch querymessages
    1. Verify that the message is retrieved and displayed to the console. 
        '));
        }
    },

    keep_in_touch : function() {
        if (automatedTestEnvironment()){
        }else{
            prompt(twiki('---++++ Keep In Touch
    For WinCE ensure that Microsoft Active Sync has enabled the syncing of the Inbox and Sent Email folders

    1. Send messages using SendMessage
    1. Change system date to other months and years, and send more messages
    1. Repeat 1, 2 for several times
    1. Launch keepintouch
    1. Select an item from *Contacted within the last* combo box
    1. Click on Search
    1. Select from Addresses list box
    1. Verify the correct messages are shown in Messages list box 
        '));
        }
    }
}
