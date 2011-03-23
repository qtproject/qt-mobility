/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#include "qdeclarativeorganizeritemdetail_p.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qdeclarativeorganizeritemmetaobject_p.h"
#include "qdeclarativeorganizermodel_p.h"

/*!
    \qmlclass OrganizerItem QDeclarativeOrganizerItem

    \brief The OrganizerItem element represents the in-memory version of a calendar organizer item,
  and has no tie to a specific backend calendar store.

    \ingroup qml-organizer

  A OrganizerItem has a number of mandatory details. Different subclasses of OrganizerItem
  (i.e., Event, EventOccurrence, Journal, Todo, TodoOccurrence, Note) may have more mandatory details.

    If some of the organizer item details are non-unique, all of this type of detail values
    can be accessed by dynamic properties. For example, there are 3 comment details stored in
    an event item, they can be accessed by event.comments property, which holds a list of
    all comment details. If the dynamic property does not exist (for unique details), an undefined
    value is returned. The list of dynamic detail properties depends on the engine implementations.

  \sa Event, EventOccurrence, Journal, Todo, TodoOccurrence, Note, {QOrganizerManager}, {QOrganizerItem}

  The OrganizerItem element is part of the \bold{QtMobility.organizer 1.1} module.
*/




QDeclarativeOrganizerItem::QDeclarativeOrganizerItem(QObject *parent)
    :QObject(parent),
    d(new QDeclarativeOrganizerItemMetaObject(this, QOrganizerItem()))
{
    d->setMetaObject(QDeclarativeOrganizerItem::staticMetaObject);
    connect(this, SIGNAL(itemChanged()), SLOT(setModified()));
}


QDeclarativeOrganizerItem::QDeclarativeOrganizerItem(const QOrganizerItem& item, const QMap<QString, QOrganizerItemDetailDefinition>& defs, QObject *parent)
    :QObject(parent),
    d(new QDeclarativeOrganizerItemMetaObject(this, item))
{
    d->setMetaObject(QDeclarativeOrganizerItem::staticMetaObject);
    setDetailDefinitions(defs);
    connect(this, SIGNAL(itemChanged()), SLOT(setModified()));
}

QDeclarativeOrganizerItem::~QDeclarativeOrganizerItem()
{
    delete d;
}

void QDeclarativeOrganizerItem::setDetailDefinitions(const QMap<QString, QOrganizerItemDetailDefinition>& defs)
{
    d->m_defs = defs;
}

QMap<QString, QOrganizerItemDetailDefinition> QDeclarativeOrganizerItem::detailDefinitions() const
{
    return d->m_defs;
}

void QDeclarativeOrganizerItem::setItem(const QOrganizerItem& item)
{
   d->setItem(item);
   emit itemChanged ();
   d->m_modified = false;
}

QOrganizerItem QDeclarativeOrganizerItem::item() const
{
    return d->item();
}


/*!
  \qmlproperty string OrganizerItem::itemId

   This property holds the id of the OrganizerItem object.
   This property is read only.
  */
QString QDeclarativeOrganizerItem::itemId() const
{
    return d->itemId();
}

/*!
  \qmlproperty string OrganizerItem::manager

  This property holds the manager uri which the \l OrganizerItem object comes from.
  */
QString QDeclarativeOrganizerItem::manager() const
{
    return d->m_item.id().managerUri();
}

/*!
  \qmlproperty bool OrganizerItem::modified

   This property holds the dirty flag of the OrganizerItem object.
   If the OrganizerItem has been changed, returns true, otherwise returns false.

   \sa OrganizerItem::save
  */
bool QDeclarativeOrganizerItem::modified() const
{
    return d->m_modified;
}

/*!
  \qmlproperty bool OrganizerItem::isFloatingTime

   This property indicates whether the organizer item created with floating date time.
   A floating time will always appear with the same value regardless of what time zone the user is in.
   A non-floating (absolute) time represents the same time regardless of the time zone,
   but will appear to change in value if the user's time zone changes.
   This property is read only.

   \since organizer 1.1.1
  */
bool QDeclarativeOrganizerItem::isFloatingTime() const
{
    switch (itemType()) {
    case QDeclarativeOrganizerItem::Event:
    case QDeclarativeOrganizerItem::EventOccurrence:
        return d->m_item.detail<QOrganizerEventTime>().startDateTime().timeSpec() == Qt::LocalTime;
    case QDeclarativeOrganizerItem::Todo:
    case QDeclarativeOrganizerItem::TodoOccurrence:
        return d->m_item.detail<QOrganizerTodoTime>().startDateTime().timeSpec() == Qt::LocalTime;
    case QDeclarativeOrganizerItem::Journal:
        return d->m_item.detail<QOrganizerJournalTime>().entryDateTime().timeSpec() == Qt::LocalTime;
    case QDeclarativeOrganizerItem::Note:
    default:
        break;
    }
    return d->m_item.detail<QOrganizerItemTimestamp>().created().timeSpec() == Qt::LocalTime;
}

/*!
  \qmlproperty date OrganizerItem::itemStartTime

   This property holds the start date time of the OrganizerItem object.
   For differrent organizer item type, the return value is differrent, too.

   \since organizer 1.1.1
   \sa OrganizerItemType
  */
QDateTime QDeclarativeOrganizerItem::itemStartTime() const
{
    switch (itemType()) {
    case QDeclarativeOrganizerItem::Event:
        return static_cast<const QDeclarativeOrganizerEvent*>(this)->startDateTime();
    case QDeclarativeOrganizerItem::EventOccurrence:
        return static_cast<const QDeclarativeOrganizerEventOccurrence*>(this)->startDateTime();
    case QDeclarativeOrganizerItem::Todo:
        return static_cast<const QDeclarativeOrganizerTodo*>(this)->startDateTime();
    case QDeclarativeOrganizerItem::TodoOccurrence:
        return static_cast<const QDeclarativeOrganizerTodoOccurrence*>(this)->startDateTime();
    case QDeclarativeOrganizerItem::Journal:
        return static_cast<const QDeclarativeOrganizerJournal*>(this)->dateTime();
    case QDeclarativeOrganizerItem::Note:
    default:
        break;
    }
    return item().detail<QOrganizerItemTimestamp>().created().toLocalTime();
}

/*!
  \qmlproperty date OrganizerItem::itemEndTime

   This property holds the end date time of the OrganizerItem object.
   For differrent organizer item type, the return value is differrent, too.

   \since organizer 1.1.1
   \sa OrganizerItemType
  */

QDateTime QDeclarativeOrganizerItem::itemEndTime() const
{
    switch (itemType()) {
    case QDeclarativeOrganizerItem::Event:
        return static_cast<const QDeclarativeOrganizerEvent*>(this)->endDateTime();
    case QDeclarativeOrganizerItem::EventOccurrence:
        return static_cast<const QDeclarativeOrganizerEventOccurrence*>(this)->endDateTime();
    case QDeclarativeOrganizerItem::Todo:
        return static_cast<const QDeclarativeOrganizerTodo*>(this)->dueDateTime();
    case QDeclarativeOrganizerItem::TodoOccurrence:
        return static_cast<const QDeclarativeOrganizerTodoOccurrence*>(this)->dueDateTime();
    case QDeclarativeOrganizerItem::Journal:
        //there is no end time for journal item,  make it 30mins later for display purpose
        return static_cast<const QDeclarativeOrganizerJournal*>(this)->dateTime().addSecs(60*30);
    case QDeclarativeOrganizerItem::Note:
    default:
        break;
    }
    //there is no end time for note or customized items,  make it 30mins later for display purpose
    return item().detail<QOrganizerItemTimestamp>().created().toLocalTime().addSecs(60*30);
}


void QDeclarativeOrganizerItem::setModified()
{
    d->m_modified = true;
}

/*!
  \qmlmethod Detail OrganizerItem::detail(name)

    This method returns an ItemDetail object which detail name is \a name.
  */
QVariant QDeclarativeOrganizerItem::detail(const QString& name)
{
    return d->detail(name);
}

/*!
    \qmlproperty list<Detail> OrganizerItem::details

    This property holds the list of \l ItemDetail elements that the organizer item has.
*/
QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> QDeclarativeOrganizerItem::details()
{
    return d->details(QString()).value< QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> >();
}

/*!
  \qmlmethod list<Detail> OrganizerItem::details(name)

    This method returns a list of details which detail name is \a name.
  */
QVariant QDeclarativeOrganizerItem::details(const QString& name)
{
    return d->details(name);
}

/*!
    \qmlmethod OrganizerItem::addComment(comment)

    Addes a \a comment for the organizer item.

    \sa OrganizerItem::clearComments()
*/
void QDeclarativeOrganizerItem::addComment(const QString& comment)
{
    d->m_item.addComment(comment);
    emit itemChanged();
}

/*!
    \qmlmethod OrganizerItem::clearComments()

    Removes all comments from the organizer item.

    \sa OrganizerItem::addComment()
*/
void QDeclarativeOrganizerItem::clearComments()
{
    d->m_item.clearComments();
    emit itemChanged();
}

/*!
    \qmlmethod bool OrganizerItem::removeDetail(detail)

    Removes given \a detail from the organizer item. If the detail is not removable, returns false.

    \sa ItemDetail::removable
*/
bool QDeclarativeOrganizerItem::removeDetail(QDeclarativeOrganizerItemDetail* detail)
{
    if (detail->removable()) {
        d->m_details.removeAll(detail);
        emit itemChanged();
        return true;
    }
    return false;

}

/*!
    \qmlmethod bool OrganizerItem::addDetail(detail)

    Adds the given organizer item \a detail to the organizer item, returns true if successful, otherwise returns false.

    \note: If the \a detail has been added into the same organizer item before, this operation will be ignored,
    so if you want to add a \a detail multiple times, the \a detail should be copied before calling this function.
*/
bool QDeclarativeOrganizerItem::addDetail(QDeclarativeOrganizerItemDetail* detail)
{
    if (detail) {
        if (!d->m_details.contains(detail)) {
            d->m_details.append(detail);
            emit itemChanged();
        }
        return true;
    }
    return false;
}

/*!
    \qmlmethod OrganizerItem::clearDetails()

    Removes all details from the organizer item.

    \sa OrganizerItem::removeDetail()
*/
void QDeclarativeOrganizerItem::clearDetails()
{
    d->m_item.clearDetails();
    d->m_details.clear();
    emit itemChanged();
}

/*!
    \qmlmethod OrganizerItem::save()

    Saves this OrganizerItem if the item has been modified.

    \sa  OrganizerItem::modified
*/
void QDeclarativeOrganizerItem::save()
{
    if (modified()) {
        QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(parent());
        if (model) {
            model->saveItem(this);
        }
    }
}

/*!
    \qmlproperty string OrganizerItem::type

    This property holds the type name of the organizer item.
    This property is read only.
*/
QString QDeclarativeOrganizerItem::type() const
{
    return d->m_item.type();
}

QDeclarativeOrganizerItem::OrganizerItemType QDeclarativeOrganizerItem::itemType() const
{
    if (d->m_item.type() == QOrganizerItemType::TypeEvent)
        return QDeclarativeOrganizerItem::Event;
    else if (d->m_item.type() == QOrganizerItemType::TypeEventOccurrence)
        return QDeclarativeOrganizerItem::EventOccurrence;
    else if (d->m_item.type() == QOrganizerItemType::TypeTodo)
        return QDeclarativeOrganizerItem::Todo;
    else if (d->m_item.type() == QOrganizerItemType::TypeTodoOccurrence)
        return QDeclarativeOrganizerItem::TodoOccurrence;
    else if (d->m_item.type() == QOrganizerItemType::TypeJournal)
        return QDeclarativeOrganizerItem::Journal;
    else if (d->m_item.type() == QOrganizerItemType::TypeNote)
        return QDeclarativeOrganizerItem::Note;
    return QDeclarativeOrganizerItem::Customized;
}

/*!
    \qmlproperty string OrganizerItem::displayLabel

    This property holds the display label of the organizer item.
*/
QString QDeclarativeOrganizerItem::displayLabel() const
{
    QDeclarativeOrganizerItemDisplayLabel* dl = d->detail<QDeclarativeOrganizerItemDisplayLabel>();
    if (dl)
        return dl->label();
    return QString();
}

void QDeclarativeOrganizerItem::setDisplayLabel(const QString& label)
{
    QDeclarativeOrganizerItemDisplayLabel* dl = d->detail<QDeclarativeOrganizerItemDisplayLabel>();
    if (dl)
        dl->setLabel(label);
}

/*!
    \qmlproperty string OrganizerItem::description

    This property holds the description text of the organizer item.
*/
QString QDeclarativeOrganizerItem::description() const
{
    QDeclarativeOrganizerItemDescription* desc = d->detail<QDeclarativeOrganizerItemDescription>();
    if (desc)
        return desc->description();
    return QString();
}

void QDeclarativeOrganizerItem::setDescription(const QString& description)
{
    QDeclarativeOrganizerItemDescription* desc = d->detail<QDeclarativeOrganizerItemDescription>();
    if (desc)
        desc->setDescription(description);
}

/*!
    \qmlproperty string OrganizerItem::guid

    This property holds the GUID string of the organizer item.
*/
QString QDeclarativeOrganizerItem::guid() const
{
    QDeclarativeOrganizerItemGuid* id = d->detail<QDeclarativeOrganizerItemGuid>();
    if (id)
        return id->guid();
    return QString();
}
void QDeclarativeOrganizerItem::setGuid(const QString& guid)
{
    QDeclarativeOrganizerItemGuid* id = d->detail<QDeclarativeOrganizerItemGuid>();
    if (id)
        id->setGuid(guid);
}

/*!
    \qmlproperty bool OrganizerItem::isOccurrence

    If this OrganizerItem is an occurrence item, returns true, otherwise returns false.

    This is a read only property.
    \since organizer 1.1.1
*/
bool QDeclarativeOrganizerItem::isOccurrence() const
{
      return itemType() == QDeclarativeOrganizerItem::EventOccurrence || itemType() == QDeclarativeOrganizerItem::TodoOccurrence;
}
///////////////////////QDeclarativeOrganizerEvent////////////////////////////////////

/*!
    \qmlclass Event QDeclarativeOrganizerEvent

    \brief The Event element provides an event in time which may reoccur.

    \ingroup qml-organizer

    \sa OrganizerItem, EventOccurrence, Journal, Todo, TodoOccurrence, Note, {QOrganizerEvent}

  The Event element is part of the \bold{QtMobility.organizer 1.1} module.
*/
QDeclarativeOrganizerEvent::QDeclarativeOrganizerEvent(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerEvent());
    d->setMetaObject(QDeclarativeOrganizerEvent::staticMetaObject);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date Event::startDateTime

  This property holds the start date time of the event.
  */
void QDeclarativeOrganizerEvent::setStartDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        time->setStartDateTime(datetime);
}

QDateTime QDeclarativeOrganizerEvent::startDateTime() const
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        return time->startDateTime();
    return QDateTime();
}

/*!
  \qmlproperty date Event::endDateTime

  This property holds the end date time of the event.
  */

void QDeclarativeOrganizerEvent::setEndDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        time->setEndDateTime(datetime);
}
QDateTime QDeclarativeOrganizerEvent::endDateTime() const
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        return time->endDateTime();
    return QDateTime();
}

/*!
  \qmlproperty bool Event::allDay

  This property indicates whether the time-of-day component of the event's start date-time or end date-time is
  insignificant. If allDay is true, the time-of-day component is considered insignificant, and the event will
  be an all-day item.
  */
void QDeclarativeOrganizerEvent::setAllDay(bool allDay)
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        time->setAllDay(allDay);
}
bool QDeclarativeOrganizerEvent::isAllDay() const
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    return time->isAllDay();
}

/*!
  \qmlproperty enumeration Event::priority

  This property holds the priority of the event. The value can be one of:
  \list
  \o Priority.Unknown
  \o Priority.Highest
  \o Priority.ExtremelyHigh
  \o Priority.VeryHigh
  \o Priority.High
  \o Priority.Medium
  \o Priority.Low
  \o Priority.VeryLow
  \o Priority.ExtremelyLow
  \o Priority.Lowest
  \endlist
  \sa Priority
  */
void QDeclarativeOrganizerEvent::setPriority(QDeclarativeOrganizerItemPriority::PriorityType value)
{
    QDeclarativeOrganizerItemPriority* priority = d->detail<QDeclarativeOrganizerItemPriority>();
    if (priority)
        priority->setPriority(value);
}
QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerEvent::priority() const
{
    QDeclarativeOrganizerItemPriority* priority = d->detail<QDeclarativeOrganizerItemPriority>();
    if (priority)
        return priority->priority();
    return QDeclarativeOrganizerItemPriority::Unknown;
}

/*!
  \qmlproperty string Event::location

  This property holds the label of the location at which the event occurs.
  */
QString QDeclarativeOrganizerEvent::location() const
{
    QDeclarativeOrganizerItemLocation* loc = d->detail<QDeclarativeOrganizerItemLocation>();
    if (loc)
        return loc->label();
    return QString();
}
void QDeclarativeOrganizerEvent::setLocation(const QString& newLocation)
{
    QDeclarativeOrganizerItemLocation* loc = d->detail<QDeclarativeOrganizerItemLocation>();
    if (loc)
        loc->setLabel(newLocation);
}

/*!
  \qmlproperty Recurrence Event::recurrence

  This property holds the recurrence element of the event item.
  */
QDeclarativeOrganizerItemRecurrence* QDeclarativeOrganizerEvent::recurrence()
{
    QDeclarativeOrganizerItemDetail* detail = d->detail(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
    return static_cast<QDeclarativeOrganizerItemRecurrence*>(detail);
}


///////////////////////////QDeclarativeOrganizerEventOccurrence////////////////////////////
/*!
    \qmlclass EventOccurrence QDeclarativeOrganizerEventOccurrence

    \brief The EventOccurrence element provides an occurrence of an event.

    \ingroup qml-organizer

    \sa OrganizerItem, Event, Journal, Todo, TodoOccurrence, Note, {QOrganizerEventOccurrence}

  The EventOccurrence element is part of the \bold{QtMobility.organizer 1.1} module.
*/

QDeclarativeOrganizerEventOccurrence::QDeclarativeOrganizerEventOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerEventOccurrence());
    d->setMetaObject(QDeclarativeOrganizerEventOccurrence::staticMetaObject);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date EventOccurrence::originalDate

  This property holds the date at which the occurrence was originally going to occur.
  */
void QDeclarativeOrganizerEventOccurrence::setOriginalDate(const QDate& date)
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        parent->setOriginalDate(date);
}

QDate QDeclarativeOrganizerEventOccurrence::originalDate() const
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        return parent->originalDate();
    return QDate();
}

/*!
  \qmlproperty date EventOccurrence::startDateTime

  This property holds the start date time of the event occurrence.
  */
void QDeclarativeOrganizerEventOccurrence::setStartDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        time->setStartDateTime(datetime);
}

QDateTime QDeclarativeOrganizerEventOccurrence::startDateTime() const
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    return time->startDateTime();
}

/*!
  \qmlproperty int EventOccurrence::parentId

  This property holds the id of the event which is this occurrence's parent.
  */
void QDeclarativeOrganizerEventOccurrence::setParentId(const QString& pid)
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        parent->setParentId(pid);
}

QString QDeclarativeOrganizerEventOccurrence::parentId() const
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        return parent->parentId();
    return QString();
}


/*!
  \qmlproperty date EventOccurrence::endDateTime

  This property holds the date time at which the event occurrence ends.
  */
void QDeclarativeOrganizerEventOccurrence::setEndDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    if (time)
        time->setEndDateTime(datetime);
}
QDateTime QDeclarativeOrganizerEventOccurrence::endDateTime() const
{
    QDeclarativeOrganizerEventTime* time = d->detail<QDeclarativeOrganizerEventTime>();
    return time->endDateTime();
}

/*!
  \qmlproperty enumeration EventOccurrence::priority

  This property holds the priority of the event occurrence. The value can be one of:
  \list
  \o Priority.Unknown
  \o Priority.Highest
  \o Priority.ExtremelyHigh
  \o Priority.VeryHigh
  \o Priority.High
  \o Priority.Medium
  \o Priority.Low
  \o Priority.VeryLow
  \o Priority.ExtremelyLow
  \o Priority.Lowest
  \endlist
  \sa Priority
  */
void QDeclarativeOrganizerEventOccurrence::setPriority(QDeclarativeOrganizerItemPriority::PriorityType value)
{
    QDeclarativeOrganizerItemPriority* priority = d->detail<QDeclarativeOrganizerItemPriority>();
    if (priority)
        priority->setPriority(value);
}
QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerEventOccurrence::priority() const
{
    QDeclarativeOrganizerItemPriority* priority = d->detail<QDeclarativeOrganizerItemPriority>();
    if (priority)
        return priority->priority();
    return QDeclarativeOrganizerItemPriority::Unknown;
}

/*!
  \qmlproperty string EventOccurrence::location

  This property holds the label of the location at which the event occurrence is held.
  */
QString QDeclarativeOrganizerEventOccurrence::location() const
{
    QDeclarativeOrganizerItemLocation* loc = d->detail<QDeclarativeOrganizerItemLocation>();
    if (loc)
        return loc->label();
    return QString();
}
void QDeclarativeOrganizerEventOccurrence::setLocation(const QString& newLocation)
{
    QDeclarativeOrganizerItemLocation* loc = d->detail<QDeclarativeOrganizerItemLocation>();
    if (loc)
        loc->setLabel(newLocation);
}
////////////////////////////////QDeclarativeOrganizerJournal////////////////////////////////////

/*!
    \qmlclass Journal QDeclarativeOrganizerJournal

    \brief The Journal element provides a journal which is associated with a particular point in time.

    \ingroup qml-organizer

    \sa OrganizerItem, Event, EventOccurrence, Todo, TodoOccurrence, Note, {QOrganizerJournal}

  The Journal element is part of the \bold{QtMobility.organizer 1.1} module.
*/

QDeclarativeOrganizerJournal::QDeclarativeOrganizerJournal(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerJournal());
    d->setMetaObject(QDeclarativeOrganizerJournal::staticMetaObject);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date Journal::dateTime

  This property holds the date time associated with this journal.
  */
void QDeclarativeOrganizerJournal::setDateTime(const QDateTime& dt)
{
    QDeclarativeOrganizerJournalTime* time = d->detail<QDeclarativeOrganizerJournalTime>();
    if (time)
        time->setEntryDateTime(dt);
}

QDateTime QDeclarativeOrganizerJournal::dateTime() const
{
    QDeclarativeOrganizerJournalTime* time = d->detail<QDeclarativeOrganizerJournalTime>();
    if (time)
        return time->entryDateTime();
    return QDateTime();
}

////////////////////QDeclarativeOrganizerNote////////////////////////
/*!
    \qmlclass Note QDeclarativeOrganizerNote

    \brief The Note element provides a note which is not associated with any particular point in time.

    \ingroup qml-organizer

    \sa OrganizerItem, Event, EventOccurrence, Journal, Todo, TodoOccurrence, {QOrganizerNote}

  The Note element is part of the \bold{QtMobility.organizer 1.1} module.
*/


QDeclarativeOrganizerNote::QDeclarativeOrganizerNote(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerNote());

    d->setMetaObject(QDeclarativeOrganizerNote::staticMetaObject);
}

//////////////////////QDeclarativeOrganizerTodo////////////////////////////////
/*!
    \qmlclass Todo QDeclarativeOrganizerTodo

    \brief The Todo element provides a task which should be completed.

    \ingroup qml-organizer

    \sa OrganizerItem, Event, EventOccurrence, Journal, TodoOccurrence, Note, {QOrganizerTodo}

  The Todo element is part of the \bold{QtMobility.organizer 1.1} module.
*/

QDeclarativeOrganizerTodo::QDeclarativeOrganizerTodo(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerTodo());
    d->setMetaObject(QDeclarativeOrganizerTodo::staticMetaObject);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty Recurrence Todo::recurrence

  This property holds the recurrence element of the todo item.
  */
QDeclarativeOrganizerItemRecurrence* QDeclarativeOrganizerTodo::recurrence()
{
    QDeclarativeOrganizerItemDetail* detail = d->detail(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
    return static_cast<QDeclarativeOrganizerItemRecurrence*>(detail);
}

/*!
  \qmlproperty date Todo::startDateTime

  This property holds the date time at which the task should be started.
  */
void QDeclarativeOrganizerTodo::setStartDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        time->setStartDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodo::startDateTime() const
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        return time->startDateTime();
    return QDateTime();
}

/*!
  \qmlproperty date Todo::dueDateTime

  This property holds the date time by which the task should be completed.
  */
void QDeclarativeOrganizerTodo::setDueDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        time->setDueDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodo::dueDateTime() const
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        return time->dueDateTime();
    return QDateTime();
}

/*!
  \qmlproperty bool Todo::allDay

  This property indicates whether the time-of-day component of the Todo's start date-time or due date-time is
  insignificant. If allDay is true, the time-of-day component is considered insignificant, and the todo will
  be an all-day item.
  */
void QDeclarativeOrganizerTodo::setAllDay(bool allDay)
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        time->setAllDay(allDay);
}

bool QDeclarativeOrganizerTodo::isAllDay() const
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        return time->isAllDay();
    return false;
}

/*!
  \qmlproperty enumeration Todo::priority

  This property holds the priority of the todo item. The value can be one of:
  \list
  \o Priority.Unknown
  \o Priority.Highest
  \o Priority.ExtremelyHigh
  \o Priority.VeryHigh
  \o Priority.High
  \o Priority.Medium
  \o Priority.Low
  \o Priority.VeryLow
  \o Priority.ExtremelyLow
  \o Priority.Lowest
  \endlist
  \sa Priority
  */
void QDeclarativeOrganizerTodo::setPriority(QDeclarativeOrganizerItemPriority::PriorityType value)
{
    QDeclarativeOrganizerItemPriority* pri = d->detail<QDeclarativeOrganizerItemPriority>();
    if (pri)
        pri->setPriority(value);
}

QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerTodo::priority() const
{
    QDeclarativeOrganizerItemPriority* pri = d->detail<QDeclarativeOrganizerItemPriority>();
    if (pri)
        return pri->priority();
    return QDeclarativeOrganizerItemPriority::Unknown;
}

/*!
  \qmlproperty int Todo::progressPercentage

  This property holds the percentage of progress completed on the task described by the todo item.
  */
void QDeclarativeOrganizerTodo::setProgressPercentage(int percentage)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setPercentageComplete(percentage);
}

int QDeclarativeOrganizerTodo::progressPercentage() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->percentageComplete();
    return 0;
}


/*!
  \qmlproperty enumeration Todo::status

  This property holds the progress status of the task described by the todo. The value can be one of:
  \list
  \o TodoProgress.NotStarted
  \o TodoProgress.InProgress
  \o TodoProgress.Complete
  \endlist
  \sa TodoProgress
  */
void QDeclarativeOrganizerTodo::setStatus(QDeclarativeOrganizerTodoProgress::StatusType value)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setStatus(value);
}

QDeclarativeOrganizerTodoProgress::StatusType QDeclarativeOrganizerTodo::status() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->status();
    return QDeclarativeOrganizerTodoProgress::NotStarted;
}

/*!
  \qmlproperty date Todo::finishedDateTime

  This property holds the date and time at which the task was completed, if known.
  */
void QDeclarativeOrganizerTodo::setFinishedDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setFinishedDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodo::finishedDateTime() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->finishedDateTime();
    return QDateTime();
}

//////////////////////////QDeclarativeOrganizerTodoOccurrence////////////////////////////////
/*!
    \qmlclass TodoOccurrence QDeclarativeOrganizerTodoOccurrence

    \brief The TodoOccurrence element provides an occurrence of an event.

    \ingroup qml-organizer

    \sa OrganizerItem, Event, EventOccurrence, Journal, Todo, Note, {QOrganizerTodoOccurrence}

  The TodoOccurrence element is part of the \bold{QtMobility.organizer 1.1} module.
*/
QDeclarativeOrganizerTodoOccurrence::QDeclarativeOrganizerTodoOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    setItem (QOrganizerTodoOccurrence());
    d->setMetaObject(QDeclarativeOrganizerTodoOccurrence::staticMetaObject);

    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date TodoOccurrence::startDateTime

  This property holds the date time at which the task should be started.
  */
void QDeclarativeOrganizerTodoOccurrence::setStartDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        time->setStartDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodoOccurrence::startDateTime() const
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        return time->startDateTime();
    return QDateTime();
}
/*!
  \qmlproperty date TodoOccurrence::dueDateTime

  This property holds the date time by which the task should be completed.
  */
void QDeclarativeOrganizerTodoOccurrence::setDueDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        time->setDueDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodoOccurrence::dueDateTime() const
{
    QDeclarativeOrganizerTodoTime* time = d->detail<QDeclarativeOrganizerTodoTime>();
    if (time)
        return time->dueDateTime();
    return QDateTime();
}

/*!
  \qmlproperty enumeration TodoOccurrence::priority

  This property holds the priority of the todo occurrence. The value can be one of:
  \list
  \o Priority.Unknown
  \o Priority.Highest
  \o Priority.ExtremelyHigh
  \o Priority.VeryHigh
  \o Priority.High
  \o Priority.Medium
  \o Priority.Low
  \o Priority.VeryLow
  \o Priority.ExtremelyLow
  \o Priority.Lowest
  \endlist
  \sa Priority
  */
void QDeclarativeOrganizerTodoOccurrence::setPriority(QDeclarativeOrganizerItemPriority::PriorityType value)
{
    QDeclarativeOrganizerItemPriority* pri = d->detail<QDeclarativeOrganizerItemPriority>();
    if (pri)
        pri->setPriority(value);
}

QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerTodoOccurrence::priority() const
{
    QDeclarativeOrganizerItemPriority* pri = d->detail<QDeclarativeOrganizerItemPriority>();
    if (pri)
        return pri->priority();
    return QDeclarativeOrganizerItemPriority::Unknown;
}

/*!
  \qmlproperty int TodoOccurrence::progressPercentage

  This property holds the percentage of progress completed on the task described by the todo item.
  */
void QDeclarativeOrganizerTodoOccurrence::setProgressPercentage(int percentage)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setPercentageComplete(percentage);
}

int QDeclarativeOrganizerTodoOccurrence::progressPercentage() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->percentageComplete();
    return 0;
}

/*!
  \qmlproperty enumeration TodoOccurrence::status

  This property holds the progress status of the task described by the todo occurrence. The value can be one of:
  \list
  \o TodoProgress.NotStarted
  \o TodoProgress.InProgress
  \o TodoProgress.Complete
  \endlist
  \sa TodoProgress
  */
void QDeclarativeOrganizerTodoOccurrence::setStatus(QDeclarativeOrganizerTodoProgress::StatusType value)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setStatus(value);
}

QDeclarativeOrganizerTodoProgress::StatusType QDeclarativeOrganizerTodoOccurrence::status() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->status();
    return QDeclarativeOrganizerTodoProgress::NotStarted;
}

/*!
  \qmlproperty date TodoOccurrence::finishedDateTime

  This property holds the date and time at which the task was completed, if known.
  */
void QDeclarativeOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& datetime)
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        progress->setFinishedDateTime(datetime);
}

QDateTime QDeclarativeOrganizerTodoOccurrence::finishedDateTime() const
{
    QDeclarativeOrganizerTodoProgress* progress = d->detail<QDeclarativeOrganizerTodoProgress>();
    if (progress)
        return progress->finishedDateTime();
    return QDateTime();
}

/*!
  \qmlproperty int TodoOccurrence::parentId

  This property holds the id of the todo which is this occurrence's parent.
  */
void QDeclarativeOrganizerTodoOccurrence::setParentId(const QString& pid)
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        parent->setParentId(pid);
}

QString QDeclarativeOrganizerTodoOccurrence::parentId() const
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        return parent->parentId();
    return QString();
}
/*!
  \qmlproperty date TodoOccurrence::originalDate

  This property holds the date at which the occurrence was originally going to occur.
  */
void QDeclarativeOrganizerTodoOccurrence::setOriginalDate(const QDate& date)
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        parent->setOriginalDate(date);
}

QDate QDeclarativeOrganizerTodoOccurrence::originalDate() const
{
    QDeclarativeOrganizerItemParent* parent = d->detail<QDeclarativeOrganizerItemParent>();
    if (parent)
        return parent->originalDate();
    return QDate();
}

Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemName, "event");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemGroupName, "events");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemName, "eventOccurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemGroupName, "eventOccurrences");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournal::ItemName, "journal");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournal::ItemGroupName, "journals");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerNote::ItemName, "note");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerNote::ItemGroupName, "notes");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodo::ItemName, "todo");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodo::ItemGroupName, "todos");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoOccurrence::ItemName, "todoOccurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoOccurrence::ItemGroupName, "todoOccurrences");
