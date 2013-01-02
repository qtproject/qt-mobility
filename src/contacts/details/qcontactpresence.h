/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTPRESENCE_H
#define QCONTACTPRESENCE_H

#include <QString>
#include <QUrl>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactPresence : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldTimestamp;
    static const QLatin1Constant FieldNickname;
    static const QLatin1Constant FieldPresenceState;
    static const QLatin1Constant FieldPresenceStateText;
    static const QLatin1Constant FieldPresenceStateImageUrl;
    static const QLatin1Constant FieldCustomMessage;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPresence, "Presence")
    Q_DECLARE_LATIN1_CONSTANT(FieldTimestamp, "Timestamp");
    Q_DECLARE_LATIN1_CONSTANT(FieldNickname, "Nickname");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceState, "PresenceState");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceStateText, "PresenceStateText");
    Q_DECLARE_LATIN1_CONSTANT(FieldPresenceStateImageUrl, "PresenceStateImageUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldCustomMessage, "CustomMessage");
#endif

    enum PresenceState {
        PresenceUnknown = 0,
        PresenceAvailable,
        PresenceHidden,
        PresenceBusy,
        PresenceAway,
        PresenceExtendedAway,
        PresenceOffline,
    };

    void setTimestamp(const QDateTime& timestamp) {setValue(FieldTimestamp, timestamp);}
    QDateTime timestamp() const {return value<QDateTime>(FieldTimestamp);}
    void setNickname(const QString& nickname) {setValue(FieldNickname, nickname);}
    QString nickname() const {return value(FieldNickname);}
    void setPresenceState(PresenceState presence) {setValue(FieldPresenceState, static_cast<int>(presence));}
    PresenceState presenceState() const {return static_cast<PresenceState>(value<int>(FieldPresenceState));}
    void setPresenceStateText(const QString& presenceStateText) {setValue(FieldPresenceStateText, presenceStateText);}
    QString presenceStateText() const {return value(FieldPresenceStateText);}
    void setPresenceStateImageUrl(const QUrl& presenceStateImageUrl) {setValue(FieldPresenceStateImageUrl, presenceStateImageUrl);}
    QUrl presenceStateImageUrl() const {return value<QUrl>(FieldPresenceStateImageUrl);}
    void setCustomMessage(const QString& customMessage) {setValue(FieldCustomMessage, customMessage);}
    QString customMessage() const {return value(FieldCustomMessage);}
};

QTM_END_NAMESPACE

#endif

