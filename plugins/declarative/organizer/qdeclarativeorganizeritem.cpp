/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** item: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeorganizeritemdetail_p.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qdeclarativeorganizeritemmetaobject_p.h"
#include "qdeclarativeorganizermodel_p.h"

QDeclarativeOrganizerItem::QDeclarativeOrganizerItem(QObject *parent)
    :QObject(parent),
    d(new QDeclarativeOrganizerItemMetaObject(this, QOrganizerItem()))
{
    d->setMetaObject(QDeclarativeOrganizerItem::staticMetaObject);
}


QDeclarativeOrganizerItem::QDeclarativeOrganizerItem(const QOrganizerItem& item, const QMap<QString, QOrganizerItemDetailDefinition>& defs, QObject *parent)
    :QObject(parent),
    d(new QDeclarativeOrganizerItemMetaObject(this, item))
{
    d->setMetaObject(QDeclarativeOrganizerItem::staticMetaObject);
    setDetailDefinitions(defs);
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
}

QOrganizerItem QDeclarativeOrganizerItem::item() const
{
    return d->item();
}

QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> QDeclarativeOrganizerItem::details()
{
    return d->details(QString()).value< QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> >();
}

uint QDeclarativeOrganizerItem::itemId() const
{
    return d->itemId();
}

QString QDeclarativeOrganizerItem::manager() const
{
    return d->m_item.id().managerUri();
}

bool QDeclarativeOrganizerItem::modified() const
{
    return d->m_modified;
}

void QDeclarativeOrganizerItem::setModified(bool modified)
{
    d->m_modified = modified;
}

QVariant QDeclarativeOrganizerItem::detail(const QString& name)
{
    return d->detail(name);
}

QVariant QDeclarativeOrganizerItem::details(const QString& name)
{
    return d->details(name);
}


void QDeclarativeOrganizerItem::addComment(const QString& comment)
{
    d->m_item.addComment(comment);
    emit itemChanged();
}

void QDeclarativeOrganizerItem::clearComments()
{
    d->m_item.clearComments();
    emit itemChanged();
}

void QDeclarativeOrganizerItem::clearDetails()
{
    d->m_item.clearDetails();
    emit itemChanged();
}

QString QDeclarativeOrganizerItem::type() const
{
    return d->m_item.type();
}
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


QDeclarativeOrganizerEvent::QDeclarativeOrganizerEvent(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerEvent::staticMetaObject);
    m_event = static_cast<QOrganizerEvent*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

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
void QDeclarativeOrganizerEvent::setTimeSpecified(bool timeSpecified)
{
    if (timeSpecified != m_event->isTimeSpecified()) {
        m_event->setTimeSpecified(timeSpecified);
        emit valueChanged();
    }
}
bool QDeclarativeOrganizerEvent::isTimeSpecified() const
{
    return m_event->isTimeSpecified();
}
void QDeclarativeOrganizerEvent::setPriority(QDeclarativeOrganizerItemPriority::Priority value)
{
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_event->priority()) {
        m_event->setPriority(newPriority);
        emit valueChanged();
    }
}
QDeclarativeOrganizerItemPriority::Priority QDeclarativeOrganizerEvent::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::Priority>(m_event->priority());
}
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

QDeclarativeOrganizerItemRecurrence* QDeclarativeOrganizerEvent::recurrence()
{
    QDeclarativeOrganizerItemDetail* detail = d->detailByDefinitionName(QDeclarativeOrganizerItemRecurrence::DetailName);
    return static_cast<QDeclarativeOrganizerItemRecurrence*>(detail);
}

QDeclarativeOrganizerEventOccurrence::QDeclarativeOrganizerEventOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerEventOccurrence::staticMetaObject);
    m_eo = static_cast<QOrganizerEventOccurrence*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

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

void QDeclarativeOrganizerEventOccurrence::setParentLocalId(uint key)
{
     QOrganizerItemId itemId = QDeclarativeOrganizerModel::itemIdFromHash(key);
     if (itemId.localId() != m_eo->parentLocalId()) {
        m_eo->setParentLocalId(itemId.localId());
        emit valueChanged();
     }
}

uint QDeclarativeOrganizerEventOccurrence::parentLocalId() const
{
    return qHash(m_eo->parentLocalId());
}

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

void QDeclarativeOrganizerEventOccurrence::setPriority(QDeclarativeOrganizerItemPriority::Priority value)
{
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_eo->priority()) {
        m_eo->setPriority(newPriority);
        emit valueChanged();
    }
}
QDeclarativeOrganizerItemPriority::Priority QDeclarativeOrganizerEventOccurrence::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::Priority>(m_eo->priority());
}
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

QDeclarativeOrganizerJournal::QDeclarativeOrganizerJournal(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerJournal::staticMetaObject);
    m_journal = static_cast<QOrganizerJournal*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

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

QDeclarativeOrganizerNote::QDeclarativeOrganizerNote(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerNote::staticMetaObject);
}
QDeclarativeOrganizerTodo::QDeclarativeOrganizerTodo(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerTodo::staticMetaObject);
    m_todo = static_cast<QOrganizerTodo*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}

QDeclarativeOrganizerItemRecurrence* QDeclarativeOrganizerTodo::recurrence()
{
    QDeclarativeOrganizerItemDetail* detail = d->detailByDefinitionName(QDeclarativeOrganizerItemRecurrence::DetailName);
    return static_cast<QDeclarativeOrganizerItemRecurrence*>(detail);
}


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

void QDeclarativeOrganizerTodo::setTimeSpecified(bool isTimeSpecified)
{
    if (isTimeSpecified != m_todo->isTimeSpecified()) {
        m_todo->setTimeSpecified(isTimeSpecified);
        emit valueChanged();
    }
}

bool QDeclarativeOrganizerTodo::isTimeSpecified() const
{
    return m_todo->isTimeSpecified();
}

void QDeclarativeOrganizerTodo::setPriority(QDeclarativeOrganizerItemPriority::Priority value)
{
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_todo->priority()) {
        m_todo->setPriority(newPriority);
        emit valueChanged();
    }
}

QDeclarativeOrganizerItemPriority::Priority QDeclarativeOrganizerTodo::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::Priority>(m_todo->priority());
}
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
void QDeclarativeOrganizerTodo::setStatus(QDeclarativeOrganizerTodoProgress::Status value)
{
    QOrganizerTodoProgress::Status newStatus = static_cast<QOrganizerTodoProgress::Status>(value);

    if (newStatus != m_todo->status()) {
        m_todo->setStatus(newStatus);
        emit valueChanged();
    }
}

QDeclarativeOrganizerTodoProgress::Status QDeclarativeOrganizerTodo::status() const
{
    return static_cast<QDeclarativeOrganizerTodoProgress::Status>(m_todo->status());
}

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


QDeclarativeOrganizerTodoOccurrence::QDeclarativeOrganizerTodoOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerTodoOccurrence::staticMetaObject);
    m_to = static_cast<QOrganizerTodoOccurrence*>(&d->m_item);
    connect(this, SIGNAL(valueChanged()), SIGNAL(itemChanged()));
}


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


void QDeclarativeOrganizerTodoOccurrence::setPriority(QDeclarativeOrganizerItemPriority::Priority value)
{
    QOrganizerItemPriority::Priority newPriority = static_cast<QOrganizerItemPriority::Priority>(value);
    if (newPriority != m_to->priority()) {
        m_to->setPriority(newPriority);
        emit valueChanged();
    }
}

QDeclarativeOrganizerItemPriority::Priority QDeclarativeOrganizerTodoOccurrence::priority() const
{
    return static_cast<QDeclarativeOrganizerItemPriority::Priority>(m_to->priority());
}
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
void QDeclarativeOrganizerTodoOccurrence::setStatus(QDeclarativeOrganizerTodoProgress::Status value)
{
    QOrganizerTodoProgress::Status newStatus = static_cast<QOrganizerTodoProgress::Status>(value);

    if (newStatus != m_to->status()) {
        m_to->setStatus(newStatus);
        emit valueChanged();
    }
}

QDeclarativeOrganizerTodoProgress::Status QDeclarativeOrganizerTodoOccurrence::status() const
{
    return static_cast<QDeclarativeOrganizerTodoProgress::Status>(m_to->status());
}

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

void QDeclarativeOrganizerTodoOccurrence::setParentLocalId(uint key)
{
    QOrganizerItemId itemId = QDeclarativeOrganizerModel::itemIdFromHash(key);
    if (itemId.localId() != m_to->parentLocalId()) {
       m_to->setParentLocalId(itemId.localId());
       emit valueChanged();
    }
}

uint QDeclarativeOrganizerTodoOccurrence::parentLocalId() const
{
    return qHash(m_to->parentLocalId());
}
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
