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
#ifndef QDECLARATIVECONTACTMODEL_P_H
#define QDECLARATIVECONTACTMODEL_P_H

#include <QAbstractListModel>
#include <QDeclarativeListProperty>
#include <QDeclarativeParserStatus>

#include "qcontact.h"
#include "qdeclarativecontact_p.h"
#include "qversitreader.h"
#include "qversitwriter.h"

#include "qdeclarativecontactfetchhint_p.h"
#include "qdeclarativecontactsortorder_p.h"
#include "qdeclarativecontactfilter_p.h"

QTM_USE_NAMESPACE;
class QDeclarativeContactModelPrivate;
class QDeclarativeContactModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString manager READ manager WRITE setManager NOTIFY managerChanged)
    Q_PROPERTY(QStringList availableManagers READ availableManagers)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QDeclarativeContactFilter* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDeclarativeContactFetchHint* fetchHint READ fetchHint WRITE setFetchHint NOTIFY fetchHintChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContact> contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContactSortOrder> sortOrders READ sortOrders NOTIFY sortOrdersChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    explicit QDeclarativeContactModel(QObject *parent = 0);

    enum {
        ContactRole =  Qt::UserRole + 500
    };

    QString manager() const;
    void setManager(const QString& manager);

    QStringList availableManagers() const;
    
    QString error() const;

    QDeclarativeContactFilter* filter() const;
    void setFilter(QDeclarativeContactFilter* filter);

    QDeclarativeContactFetchHint* fetchHint() const;
    void setFetchHint(QDeclarativeContactFetchHint* fetchHint);

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);

    QDeclarativeListProperty<QDeclarativeContact> contacts() ;
    QDeclarativeListProperty<QDeclarativeContactSortOrder> sortOrders() ;

    Q_INVOKABLE void removeContact(QContactLocalId id);
    Q_INVOKABLE void saveContact(QDeclarativeContact* dc);


signals:
    void managerChanged();
    void filterChanged();
    void errorChanged();
    void fetchHintChanged();
    void contactsChanged();
    void vcardChanged();
    void sortOrdersChanged();
    void autoUpdateChanged();

public slots:
    void update();
    void exportContacts(const QUrl& url, const QStringList& profiles = QStringList());
    void importContacts(const QUrl& url, const QStringList& profiles = QStringList());
    void removeContacts(const QList<QContactLocalId>& ids);
    void fetchContacts(const QList<QContactLocalId>& contactIds);
private slots:
    void fetchAgain();
    void requestUpdated();
    void contactsSaved();
    void contactsRemoved();
    void contactsRemoved(const QList<QContactLocalId>& ids);
    void contactsChanged(const QList<QContactLocalId>& ids);

    void startImport(QVersitReader::State state);
    void contactsExported(QVersitWriter::State state);



private:
    QDeclarativeContactModelPrivate* d;
};

#endif // QDECLARATIVECONTACTMODEL_P_H
