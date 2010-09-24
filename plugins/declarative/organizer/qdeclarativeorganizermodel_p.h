/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#ifndef QDECLARATIVEORGANIZERMODEL_H
#define QDECLARATIVEORGANIZERMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeListProperty>
#include "qorganizeritem.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qversitreader.h"
#include "qversitwriter.h"

#include "qdeclarativeorganizeritemfetchhint_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizeritemfilter_p.h"

QTM_USE_NAMESPACE;
class QDeclarativeOrganizerModelPrivate;
class QDeclarativeOrganizerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString manager READ manager)
    Q_PROPERTY(QDateTime startPeriod READ startPeriod)
    Q_PROPERTY(QDateTime endPeriod READ endPeriod)
    Q_PROPERTY(QDeclarativeOrganizerItemFilter* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemFetchHint* fetchHint READ fetchHint WRITE setFetchHint NOTIFY fetchHintChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> sortOrders READ sortOrders NOTIFY sortOrdersChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> events READ events NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> eventOccurrences READ eventOccurrences NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> todos READ todos NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> todoOccurrences READ todoOccurrences NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> journals READ journals NOTIFY itemsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> notes READ notes NOTIFY itemsChanged)

public:
    enum {
        OrganizerItemIdRole = Qt::UserRole + 500,
        OrganizerItemRole

    };
    explicit QDeclarativeOrganizerModel(QObject *parent = 0);
    explicit QDeclarativeOrganizerModel(QOrganizerItemManager* manager, const QDateTime& start, const QDateTime& end, QObject *parent = 0);

    QString manager();
    void setManager(QOrganizerItemManager* manager);

    QDateTime startPeriod() const;
    void setStartPeriod(const QDateTime& start);

    QDateTime endPeriod() const;
    void setEndPeriod(const QDateTime& end);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QDeclarativeOrganizerItemFilter* filter() const;
    void setFilter(QDeclarativeOrganizerItemFilter* filter);

    QDeclarativeOrganizerItemFetchHint* fetchHint() const;
    void setFetchHint(QDeclarativeOrganizerItemFetchHint* fetchHint);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QDeclarativeListProperty<QDeclarativeOrganizerItem> items() ;
    QDeclarativeListProperty<QDeclarativeContactSortOrder> sortOrders() ;

    Q_INVOKABLE void removeItem(uint id);
    Q_INVOKABLE void removeItems(const QList<uint>& ids);
    Q_INVOKABLE void saveItem(QDeclarativeOrganizerItem* item);


signals:
    void managerChanged();
    void filterChanged();
    void fetchHintChanged();
    void itemsChanged();
    void vcardChanged();
    void sortOrdersChanged();

public slots:
    void exportItems(const QString& file);
    void importItems(const QString& file);
private slots:
    void fetchAgain();
    void contactFetched();

    void saveContact();
    void contactSaved();

    void removeContact();
    void contactRemoved();

    void startImport(QVersitReader::State state);
    void contactsExported(QVersitWriter::State state);



private:
    QDeclarativeOrganizerItemModelPrivate* d;
};

#endif // QDECLARATIVEORGANIZERMODEL_H
