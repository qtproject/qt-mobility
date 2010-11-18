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
   d->m_modified = false;
}

QOrganizerItem QDeclarativeOrganizerItem::item() const
{
    return d->item();
}


/*!
  \qmlproperty int OrganizerItem::itemId

   This property holds the id of the OrganizerItem object.
   This property is read only.
  */
uint QDeclarativeOrganizerItem::itemId() const
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

void QDeclarativeOrganizerItem::setModified()
{
    d->m_modified = true;
}

QVariant QDeclarativeOrganizerItem::detail(const QString& name)
{
    return d->detail(name);
}

/*!
    \qmlproperty QDeclarativeListProperty OrganizerItem::details

    This property holds the list of \l ItemDetail elements that the organizer item has.
*/
QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> QDeclarativeOrganizerItem::details()
{
    return d->details(QString()).value< QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> >();
}

/*!
  \qmlmethod QDeclarativeListProperty OrganizerItem::details(name)

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

/*!
    \qmlproperty string OrganizerItem::displayLabel

    This property holds the display label of the organizer item.
*/
QString QDeclarativeOrganizerItem::displayLabel() const
{
    return d->m_item.displayLabel();
}

void QDeclarativeOrganizerItem::setDisplayLabel(const QString& label)
{
    if (label != d->m_item.displayLabel()) {
        d->m_item.setDisplayLabel(label);
        emit itemChanged();
    }
}

/*!
    \qmlproperty string OrganizerItem::description

    This property holds the description text of the organizer item.
*/
QString QDeclarativeOrganizerItem::description() const
{
    return d->m_item.description();
}

void QDeclarativeOrganizerItem::setDescription(const QString& description)
{
    if (description != d->m_item.description()) {
        d->m_item.setDescription(description);
        emit itemChanged();
    }
}

/*!
    \qmlproperty string OrganizerItem::guid

    This property holds the GUID string of the organizer item.
*/
QString QDeclarativeOrganizerItem::guid() const
{
    return d->m_item.guid();
}
void QDeclarativeOrganizerItem::setGuid(const QString& guid)
{
    if (guid != d->m_item.guid()) {
        d->m_item.setGuid(guid);
        emit itemChanged();
    }
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
    d->setMetaObject(QDeclarativeOrganizerEvent::staticMetaObject);
    m_event = static_cast<QOrganizerEvent*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date Event::startDateTime

  This property holds the start date time of the event.
  */
void QDeclarativeOrganizerEvent::setStartDateTime(const QDateTime& datetime)
{
    if (datetime != m_event->startDateTime()){
        m_event->setStartDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerEvent::startDateTime() const
{
    return m_event->startDateTime();
}

/*!
  \qmlproperty date Event::endDateTime

  This property holds the end date time of the event.
  */

void QDeclarativeOrganizerEvent::setEndDateTime(const QDateTime& datetime)
{
    if (datetime != m_event->endDateTime()){
        m_event->setEndDateTime(datetime);
        emit valueChanged();
    }
}
QDateTime QDeclarativeOrganizerEvent::endDateTime() const
{
    return m_event->endDateTime();
}

/*!
  \qmlproperty bool Event::allDay

  This property indicates whether the time-of-day component of the event's start date-time or end date-time is
  insignificant. If allDay is true, the time-of-day component is considered insignificant, and the event will
  be an all-day item.
  */
void QDeclarativeOrganizerEvent::setAllDay(bool isAllDay)
{
    if (isAllDay != m_event->isAllDay()) {
        m_event->setAllDay(isAllDay);
        emit valueChanged();
    }
}
bool QDeclarativeOrganizerEvent::isAllDay() const
{
    return m_event->isAllDay();
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
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_event->priority()) {
        m_event->setPriority(newPriority);
        emit valueChanged();
    }
}
QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerEvent::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::PriorityType>(m_event->priority());
}

/*!
  \qmlproperty string Event::location

  This property holds the label of the location at which the event occurs.
  */
QString QDeclarativeOrganizerEvent::location() const
{
    return m_event->location();
}
void QDeclarativeOrganizerEvent::setLocation(const QString& loc)
{
    if (loc != m_event->location()) {
        m_event->setLocation(loc);
        emit valueChanged();
    }
}

/*!
  \qmlproperty ItemRecurrence Event::recurrence

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
    d->setMetaObject(QDeclarativeOrganizerEventOccurrence::staticMetaObject);
    m_eo = static_cast<QOrganizerEventOccurrence*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date EventOccurrence::originalDate

  This property holds the date at which the occurrence was originally going to occur.
  */
void QDeclarativeOrganizerEventOccurrence::setOriginalDate(const QDate& date)
{
    if (date != m_eo->originalDate()){
        m_eo->setOriginalDate(date);
        emit valueChanged();
    }
}

QDate QDeclarativeOrganizerEventOccurrence::originalDate() const
{
    return m_eo->originalDate();
}

/*!
  \qmlproperty date EventOccurrence::startDateTime

  This property holds the start date time of the event occurrence.
  */
void QDeclarativeOrganizerEventOccurrence::setStartDateTime(const QDateTime& datetime)
{
    if (datetime != m_eo->startDateTime()){
        m_eo->setStartDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerEventOccurrence::startDateTime() const
{
    return m_eo->startDateTime();
}

/*!
  \qmlproperty int EventOccurrence::parentId

  This property holds the id of the event which is this occurrence's parent.
  */
void QDeclarativeOrganizerEventOccurrence::setParentId(uint key)
{
     QOrganizerItemId itemId = QDeclarativeOrganizerModel::itemIdFromHash(key);
     if (itemId != m_eo->parentId()) {
        m_eo->setParentId(itemId);
        emit valueChanged();
     }
}

uint QDeclarativeOrganizerEventOccurrence::parentId() const
{
    return qHash(m_eo->parentId());
}


/*!
  \qmlproperty date EventOccurrence::endDateTime

  This property holds the date time at which the event occurrence ends.
  */
void QDeclarativeOrganizerEventOccurrence::setEndDateTime(const QDateTime& datetime)
{
    if (datetime != m_eo->endDateTime()){
        m_eo->setEndDateTime(datetime);
        emit valueChanged();
    }
}
QDateTime QDeclarativeOrganizerEventOccurrence::endDateTime() const
{
    return m_eo->endDateTime();
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
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_eo->priority()) {
        m_eo->setPriority(newPriority);
        emit valueChanged();
    }
}
QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerEventOccurrence::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::PriorityType>(m_eo->priority());
}

/*!
  \qmlproperty string EventOccurrence::location

  This property holds the label of the location at which the event occurrence is held.
  */
QString QDeclarativeOrganizerEventOccurrence::location() const
{
    return m_eo->location();
}
void QDeclarativeOrganizerEventOccurrence::setLocation(const QString& loc)
{
    if (loc != m_eo->location()) {
        m_eo->setLocation(loc);
        emit valueChanged();
    }
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
    d->setMetaObject(QDeclarativeOrganizerJournal::staticMetaObject);
    m_journal = static_cast<QOrganizerJournal*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date Journal::dateTime

  This property holds the date time associated with this journal.
  */
void QDeclarativeOrganizerJournal::setDateTime(const QDateTime& datetime)
{
    if (datetime != m_journal->dateTime()) {
        m_journal->setDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerJournal::dateTime() const
{
    return m_journal->dateTime();
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
    d->setMetaObject(QDeclarativeOrganizerTodo::staticMetaObject);
    m_todo = static_cast<QOrganizerTodo*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty ItemRecurrence Todo::recurrence

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
    if (datetime != m_todo->startDateTime()) {
        m_todo->setStartDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodo::startDateTime() const
{
    return m_todo->startDateTime();
}

/*!
  \qmlproperty date Todo::dueDateTime

  This property holds the date time by which the task should be completed.
  */
void QDeclarativeOrganizerTodo::setDueDateTime(const QDateTime& datetime)
{
    if (datetime != m_todo->dueDateTime()) {
        m_todo->setDueDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodo::dueDateTime() const
{
    return m_todo->dueDateTime();
}

/*!
  \qmlproperty bool Todo::allDay

  This property indicates whether the time-of-day component of the Todo's start date-time or due date-time is
  insignificant. If allDay is true, the time-of-day component is considered insignificant, and the todo will
  be an all-day item.
  */
void QDeclarativeOrganizerTodo::setAllDay(bool isAllDay)
{
    if (isAllDay != m_todo->isAllDay()) {
        m_todo->setAllDay(isAllDay);
        emit valueChanged();
    }
}

bool QDeclarativeOrganizerTodo::isAllDay() const
{
    return m_todo->isAllDay();
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
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_todo->priority()) {
        m_todo->setPriority(newPriority);
        emit valueChanged();
    }
}

QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerTodo::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::PriorityType>(m_todo->priority());
}

/*!
  \qmlproperty int Todo::progressPercentage

  This property holds the percentage of progress completed on the task described by the todo item.
  */
void QDeclarativeOrganizerTodo::setProgressPercentage(int percentage)
{
    if (percentage != m_todo->progressPercentage()) {
        m_todo->setProgressPercentage(percentage);
        emit valueChanged();
    }
}

int QDeclarativeOrganizerTodo::progressPercentage() const
{
    return m_todo->progressPercentage();
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
    QOrganizerTodoProgress::Status newStatus = static_cast<QOrganizerTodoProgress::Status>(value);

    if (newStatus != m_todo->status()) {
        m_todo->setStatus(newStatus);
        emit valueChanged();
    }
}

QDeclarativeOrganizerTodoProgress::StatusType QDeclarativeOrganizerTodo::status() const
{
    return static_cast<QDeclarativeOrganizerTodoProgress::StatusType>(m_todo->status());
}

/*!
  \qmlproperty date Todo::finishedDateTime

  This property holds the date and time at which the task was completed, if known.
  */
void QDeclarativeOrganizerTodo::setFinishedDateTime(const QDateTime& datetime)
{
    if (datetime != m_todo->finishedDateTime()) {
        m_todo->setFinishedDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodo::finishedDateTime() const
{
    return m_todo->finishedDateTime();
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
    d->setMetaObject(QDeclarativeOrganizerTodoOccurrence::staticMetaObject);
    m_to = static_cast<QOrganizerTodoOccurrence*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

/*!
  \qmlproperty date TodoOccurrence::startDateTime

  This property holds the date time at which the task should be started.
  */
void QDeclarativeOrganizerTodoOccurrence::setStartDateTime(const QDateTime& datetime)
{
    if (datetime != m_to->startDateTime()) {
        m_to->setStartDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodoOccurrence::startDateTime() const
{
    return m_to->startDateTime();
}
/*!
  \qmlproperty date TodoOccurrence::dueDateTime

  This property holds the date time by which the task should be completed.
  */
void QDeclarativeOrganizerTodoOccurrence::setDueDateTime(const QDateTime& datetime)
{
    if (datetime != m_to->dueDateTime()) {
        m_to->setDueDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodoOccurrence::dueDateTime() const
{
    return m_to->dueDateTime();
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
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_to->priority()) {
        m_to->setPriority(newPriority);
        emit valueChanged();
    }
}

QDeclarativeOrganizerItemPriority::PriorityType QDeclarativeOrganizerTodoOccurrence::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::PriorityType>(m_to->priority());
}

/*!
  \qmlproperty int TodoOccurrence::progressPercentage

  This property holds the percentage of progress completed on the task described by the todo item.
  */
void QDeclarativeOrganizerTodoOccurrence::setProgressPercentage(int percentage)
{
    if (percentage != m_to->progressPercentage()) {
        m_to->setProgressPercentage(percentage);
        emit valueChanged();
    }
}

int QDeclarativeOrganizerTodoOccurrence::progressPercentage() const
{
    return m_to->progressPercentage();
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
    QOrganizerTodoProgress::Status newStatus = static_cast<QOrganizerTodoProgress::Status>(value);

    if (newStatus != m_to->status()) {
        m_to->setStatus(newStatus);
        emit valueChanged();
    }
}

QDeclarativeOrganizerTodoProgress::StatusType QDeclarativeOrganizerTodoOccurrence::status() const
{
    return static_cast<QDeclarativeOrganizerTodoProgress::StatusType>(m_to->status());
}

/*!
  \qmlproperty date TodoOccurrence::finishedDateTime

  This property holds the date and time at which the task was completed, if known.
  */
void QDeclarativeOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& datetime)
{
    if (datetime != m_to->finishedDateTime()) {
        m_to->setFinishedDateTime(datetime);
        emit valueChanged();
    }
}

QDateTime QDeclarativeOrganizerTodoOccurrence::finishedDateTime() const
{
    return m_to->finishedDateTime();
}

/*!
  \qmlproperty int TodoOccurrence::parentId

  This property holds the id of the todo which is this occurrence's parent.
  */
void QDeclarativeOrganizerTodoOccurrence::setParentId(uint key)
{
    QOrganizerItemId itemId = QDeclarativeOrganizerModel::itemIdFromHash(key);
    if (itemId != m_to->parentId()) {
       m_to->setParentId(itemId);
       emit valueChanged();
    }
}

uint QDeclarativeOrganizerTodoOccurrence::parentId() const
{
    return qHash(m_to->parentId());
}

/*!
  \qmlproperty date TodoOccurrence::originalDate

  This property holds the date at which the occurrence was originally going to occur.
  */
void QDeclarativeOrganizerTodoOccurrence::setOriginalDate(const QDate& date)
{
    if (date != m_to->originalDate()) {
        m_to->setOriginalDate(date);
        emit valueChanged();
    }
}

QDate QDeclarativeOrganizerTodoOccurrence::originalDate() const
{
    return m_to->originalDate();
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
