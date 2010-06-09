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

  add_item: function()
  {
      prompt(twiki('---++ Add Item
Allows user to create an organizer item that is stored in a calendar store. The local calendar store for example.'));
  },

  delete_item: function()
  {
      prompt(twiki('---++ Delete Item
Allows user to delete an organizer item from the calendar store.'));
  },

  edit_item: function()
  {
      prompt(twiki('---++ Edit Item
Allows user to edit an existing organizer item that is stored in a calendar store.'));
  },

  search_items_by_time: function()
  {
      prompt(twiki('---++ Search Items By Time
Access all organizer items corresponding to a particular period. For example, an application that shows all organizer items occurring on the current day.'));
  },

  search_items_any_attribute: function()
  {
      prompt(twiki('---++ Search Items Using Any Attribute 	
    * Access organizer items corresponding to one or more attribute values.
    * For example a user may search for all items that occurred January to March 2009, at Brisbane where the description contains "birthday party".
'));
  },

  online_calendar: function()
  {
      prompt(twiki('---++ Search Items Using Any Attribute 	
    * Access organizer items corresponding to one or more attribute values.
    * For example a user may search for all items that occurred January to March 2009, at Brisbane where the description contains "birthday party".'));
  },

  custom_data: function()
  {
      prompt(twiki('---++Custom Data 	
    * Add custom data to an organizer item.
    * For example a travel organizer application may want to add non-standard new fields to an organizer item representing travel information.'));
  },

  multiple_stores: function()
  {
      prompt(twiki('---++ Supporting Multiple Stores 	
    * Application may want to choose between the internal calendar store and an online calendar store.
    * It is possible for there to be more than one store on the local device.
'));
  },

  item_recurrence: function()
  {
      prompt(twiki('---++ Item Recurrence
Work with recurring organizer items. For example use the API to create a fortnightly entry that lasts for 10 recurrences. Another example is working with recurring entries read from an event store. Different stores may handle recurrence differently. 	
'));
  },

  change_notification: function()
  {
      prompt(twiki('---++ Notifications of Changes in Data 	
    * An application can use the API to be notified of changed in the underlying data.
    * For example an application that shows the coming weeks organizer items would want to be notified when the underlying calendar data changes so it can check if it needs to update the displayed organizer items.'));
  },

  access_control: function()
  {
      prompt(twiki('---++ Access Control 	
    * Control who can and who cannot view organizer items.'));
  },

  import_ical: function()
  {
      prompt(twiki('---++ Import iCal 	
    * Import a iCal data into a calendar store.
    * For example, import data exported from another calendar.'));
  },

  import_vcal: function()
  {
      prompt(twiki('---++ Import vCal 	
    * Import vCal data.
    * Format used on Symbian 3.x devices.'));
  }

}

/*
---++ Access Online Calendar Store
Work with entries in an online organizer item store such as Google Calendar.
*/
