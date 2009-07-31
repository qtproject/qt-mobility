/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMESSAGESERVICEACTION_H
#define QMESSAGESERVICEACTION_H
#include <QObject>
#include <qmessageglobal.h>
#include <qmessage.h>

class QMessageServiceActionPrivate;

class Q_MESSAGING_EXPORT QMessageServiceAction : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMessageAddress)

public:
    enum Activity {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

    QMessageServiceAction(QObject *parent = 0);
    ~QMessageServiceAction();

    void send(const QMessage &message, const QMessageAccountId &accountId);
    void compose(const QMessage &message);
    void retrieveHeader(const QMessageId &id);
    void retrieveBody(const QMessageId &id);
    void retrieve(const QMessageContentContainerId &id);
    void show(const QMessageId &id);
    void exportUpdates();
    Activity activity() const;
    QString lastErrorString() const;

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    QMessageServiceActionPrivate *d_ptr;
};
#endif
