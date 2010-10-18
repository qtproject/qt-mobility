/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#ifndef QDECLARATIVECONTACTPRESENCE_H
#define QDECLARATIVECONTACTPRESENCE_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactpresence.h"

class QDeclarativeContactPresence : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY fieldsChanged)
    Q_PROPERTY(QString nickname READ nickname WRITE setNickname NOTIFY fieldsChanged)
    Q_PROPERTY(PresenceStateType state READ presenceState WRITE setPresenceState NOTIFY fieldsChanged)
    Q_PROPERTY(QString stateText READ presenceStateText WRITE setPresenceStateText NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl imageUrl READ presenceStateImageUrl WRITE setPresenceStateImageUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QString customMessage READ customMessage WRITE setCustomMessage NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(PresenceStateType)
    Q_CLASSINFO("DefaultProperty", "state")
public:
    enum FieldType {
        Timestamp = 0,
        Nickname,
        State,
        StateText,
        ImageUrl,
        CustomMessage
    };

    enum PresenceStateType {
        Unknown = QContactPresence::PresenceUnknown,
        Available = QContactPresence::PresenceAvailable,
        Hidden = QContactPresence::PresenceHidden,
        Busy = QContactPresence::PresenceBusy,
        Away = QContactPresence::PresenceAway,
        ExtendedAway = QContactPresence::PresenceExtendedAway,
        Offline = QContactPresence::PresenceOffline
    };

    QDeclarativeContactPresence(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactPresence());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactPresence;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Timestamp:
            return QContactPresence::FieldTimestamp;
        case Nickname:
            return QContactPresence::FieldNickname;
        case State:
            return QContactPresence::FieldPresenceState;
        case StateText:
            return QContactPresence::FieldPresenceStateText;
        case ImageUrl:
            return QContactPresence::FieldPresenceStateImageUrl;
        case CustomMessage:
            return QContactPresence::FieldCustomMessage;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setTimestamp(const QDateTime& v)
    {
        if (!readOnly() && v != timestamp()) {
            detail().setValue(QContactPresence::FieldTimestamp, v);
            emit fieldsChanged();
        }
    }
    QDateTime timestamp() const {return detail().value<QDateTime>(QContactPresence::FieldTimestamp);}
    void setNickname(const QString& v)
    {
        if (!readOnly() && v != nickname()) {
            detail().setValue(QContactPresence::FieldNickname, v);
            emit fieldsChanged();
        }
    }
    QString nickname() const {return detail().value(QContactPresence::FieldNickname);}

    void setPresenceState(PresenceStateType v)
    {
        if (!readOnly() && v != presenceState()) {
            detail().setValue(QContactPresence::FieldPresenceState, static_cast<int>(v));
            emit fieldsChanged();
        }
    }

    PresenceStateType presenceState() const
    {
        return static_cast<PresenceStateType>(detail().value<int>(QContactPresence::FieldPresenceState));

    }

    void setPresenceStateText(const QString& v)
    {
        if (!readOnly() && v != presenceStateText()) {
            detail().setValue(QContactPresence::FieldPresenceStateText, v);
            emit fieldsChanged();
        }
    }
    QString presenceStateText() const {return detail().value(QContactPresence::FieldPresenceStateText);}
    void setPresenceStateImageUrl(const QUrl& v)
    {
        if (!readOnly() && v != presenceStateImageUrl())  {
            detail().setValue(QContactPresence::FieldPresenceStateImageUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl presenceStateImageUrl() const {return detail().value(QContactPresence::FieldPresenceStateImageUrl);}
    void setCustomMessage(const QString& v)
    {
        if (!readOnly() && v != customMessage()) {
            detail().setValue(QContactPresence::FieldCustomMessage, v);
            emit fieldsChanged();
        }
    }
    QString customMessage() const {return detail().value(QContactPresence::FieldCustomMessage);}
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactPresence)
#endif

