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
#ifndef QDECLARATIVECONTACTRELATIONSHIPMODEL_P_H
#define QDECLARATIVECONTACTRELATIONSHIPMODEL_P_H

#include <qdeclarative.h>
#include <QAbstractListModel>

#include "qdeclarativecontactrelationship_p.h"

QTM_USE_NAMESPACE;
class QDeclarativeContactRelationshipModelPrivate;
class QDeclarativeContactRelationshipModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString manager READ manager WRITE setManager NOTIFY managerChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QContactLocalId participantId READ participantId WRITE setParticipantId NOTIFY participantIdChanged)
    Q_PROPERTY(QVariant relationshipType READ relationshipType WRITE setRelationshipType NOTIFY relationshipTypeChanged)
    Q_PROPERTY(QDeclarativeContactRelationship::RelationshipRole role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContactRelationship> relationships READ relationships NOTIFY relationshipsChanged)
    Q_PROPERTY(QString error READ error)

public:
    QDeclarativeContactRelationshipModel(QObject *parent = 0);
    ~QDeclarativeContactRelationshipModel();
    enum {
        RelationshipRole = Qt::UserRole + 500

    };

    QString manager() const;
    void setManager(const QString& manager);
    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);
    QString error() const;
    QContactLocalId participantId() const;
    void setParticipantId(const QContactLocalId& id);

    QVariant relationshipType() const;
    void setRelationshipType(const QVariant& type);

    QDeclarativeContactRelationship::RelationshipRole role() const;
    void setRole(QDeclarativeContactRelationship::RelationshipRole role);

    QDeclarativeListProperty<QDeclarativeContactRelationship> relationships();
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void removeRelationship(QDeclarativeContactRelationship* dcr);
    Q_INVOKABLE void addRelationship(QDeclarativeContactRelationship* dcr);
signals:
    void managerChanged();
    void participantIdChanged();
    void relationshipTypeChanged();
    void roleChanged();
    void relationshipsChanged();
    void autoUpdateChanged();
private slots:
    void fetchAgain();
    void requestUpdated();

    void relationshipsSaved();

    void relationshipsRemoved();

private:
    QDeclarativeContactRelationshipModelPrivate* d;
};

QML_DECLARE_TYPE(QDeclarativeContactRelationshipModel)
#endif // QDECLARATIVECONTACTRELATIONSHIPMODEL_P_H
