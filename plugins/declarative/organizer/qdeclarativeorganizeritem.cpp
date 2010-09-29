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
    emit valueChanged();
}

void QDeclarativeOrganizerItem::clearComments()
{
    d->m_item.clearComments();
    emit valueChanged();
}

void QDeclarativeOrganizerItem::clearDetails()
{
    d->m_item.clearDetails();
    emit valueChanged();
}

QDeclarativeOrganizerItemDetail* QDeclarativeOrganizerItem::detailByDefinitionName(const QString& name) const
{
    return d->detailByDefinitionName(name);
}


QDeclarativeOrganizerEvent::QDeclarativeOrganizerEvent(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerEvent::staticMetaObject);
}

QDeclarativeOrganizerEventOccurrence::QDeclarativeOrganizerEventOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerEventOccurrence::staticMetaObject);
}

QDeclarativeOrganizerJournal::QDeclarativeOrganizerJournal(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerJournal::staticMetaObject);
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
}
QDeclarativeOrganizerTodoOccurrence::QDeclarativeOrganizerTodoOccurrence(QObject *parent)
    :QDeclarativeOrganizerItem(parent)
{
    d->setMetaObject(QDeclarativeOrganizerTodoOccurrence::staticMetaObject);
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
