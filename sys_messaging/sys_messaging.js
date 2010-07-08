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

testcase = {

  send_message: function()
  {
      prompt(twiki('---++++ SendMessage
   1 Launch Debug/bin/sendmessage
   1 Fill in To, Subject, Content, and Attachment
   1 Click on Send
   1 Check if the mail has been sent to the desired mailbox'));
  },

  query_messages: function()
  {
      prompt(twiki('---++++ QueryMessages
   1 Send an email to the test account
   1 Launch Debug/bin/querymessages
   1 Verify that the message is retrieved'));
  },

  compose_send: function()
  {
      prompt(twiki('---++++ ComposeAndSend
   1 Launch Debug/bin/serviceactions
   1 Fill in Account, To, Subject, Content, Attachment
   1 Click on Send
   1 Verify the mail has been sent to the desired mailbox correctly
   1 Note: Show and Compose functions are not implemented yet'));
  },

  keep_in_touch: function()
  {
      prompt(twiki('---++++ KeepInTouch
   1 Send messages using !SendMessage
   1 Change system date to other months and years, and send more messages
   1 Repeat 1, 2 for several times
   1 Launch Debug/bin/keepintouch
   1 Select an item from "Contacted within the last" combo box
   1 Click on Search
   1 Select from Addresses list box
   1 Verify the correct messages are shown in Messages list box'));
  }

}
