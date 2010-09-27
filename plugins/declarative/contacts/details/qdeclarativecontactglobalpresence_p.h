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


#ifndef QDECLARATIVECONTACTGLOBALPRESENCE_H
#define QDECLARATIVECONTACTGLOBALPRESENCE_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactglobalpresence.h"

class QDeclarativeContactGlobalPresence : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY fieldsChanged)
    Q_PROPERTY(QString nickname READ nickname WRITE setNickname NOTIFY fieldsChanged)
    Q_PROPERTY(QString state READ presenceState WRITE setPresenceState NOTIFY fieldsChanged)
    Q_PROPERTY(QString stateText READ presenceStateText WRITE setPresenceStateText NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl imageUrl READ presenceStateImageUrl WRITE setPresenceStateImageUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QString customMessage READ customMessage WRITE setCustomMessage NOTIFY fieldsChanged)
    Q_CLASSINFO("DefaultProperty", "state")
    Q_ENUMS(FieldType);
public:
    enum FieldType {
        Timestamp = 0,
        Nickname,
        State,
        StateText,
        ImageUrl,
        CustomMessage
    };

    QDeclarativeContactGlobalPresence(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactGlobalPresence());
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::GlobalPresence;
    }

    void setTimestamp(const QDateTime& timestamp) {detail().setValue(QContactGlobalPresence::FieldTimestamp, timestamp);}
    QDateTime timestamp() const {return detail().value<QDateTime>(QContactGlobalPresence::FieldTimestamp);}
    void setNickname(const QString& nickname) {detail().setValue(QContactGlobalPresence::FieldNickname, nickname);}
    QString nickname() const {return detail().value(QContactGlobalPresence::FieldNickname);}
    void setPresenceState(QString presenceState) {
        int state = QContactPresence::PresenceUnknown;

        if (presenceState == "Available")
            state = QContactPresence::PresenceAvailable;
        else if (presenceState == "Hidden")
            state = QContactPresence::PresenceHidden;
        else if (presenceState == "Busy")
            state = QContactPresence::PresenceBusy;
        else if (presenceState == "Away")
            state = QContactPresence::PresenceAway;
        else if (presenceState == "ExtendedAway")
            state = QContactPresence::PresenceExtendedAway;
        else if (presenceState == "Offline")
            state = QContactPresence::PresenceOffline;

        detail().setValue(QContactGlobalPresence::FieldPresenceState, state);
    }

    QString presenceState() const {
        int state = detail().value<int>(QContactGlobalPresence::FieldPresenceState);
        switch (state) {
        case QContactPresence::PresenceAvailable:
            return QString::fromLatin1("Available");
        case QContactPresence::PresenceHidden:
            return QString::fromLatin1("Hidden");
        case QContactPresence::PresenceBusy:
            return QString::fromLatin1("Busy");
        case QContactPresence::PresenceAway:
            return QString::fromLatin1("Away");
        case QContactPresence::PresenceExtendedAway:
            return QString::fromLatin1("ExtendedAway");
        case QContactPresence::PresenceOffline:
            return QString::fromLatin1("Offline");
        }

        return QString::fromLatin1("Unknown");
    }
    void setPresenceStateText(const QString& presenceStateText) {detail().setValue(QContactGlobalPresence::FieldPresenceStateText, presenceStateText);}
    QString presenceStateText() const {return detail().value(QContactGlobalPresence::FieldPresenceStateText);}
    void setPresenceStateImageUrl(const QUrl& presenceStateImageUrl) {detail().setValue(QContactGlobalPresence::FieldPresenceStateImageUrl, presenceStateImageUrl);}
    QUrl presenceStateImageUrl() const {return detail().value<QUrl>(QContactGlobalPresence::FieldPresenceStateImageUrl);}
    void setCustomMessage(const QString& customMessage) {detail().setValue(QContactGlobalPresence::FieldCustomMessage, customMessage);}
    QString customMessage() const {return detail().value(QContactGlobalPresence::FieldCustomMessage);}
signals:
    void fieldsChanged();
};
QML_DECLARE_TYPE(QDeclarativeContactGlobalPresence)

#endif

