/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QDECLARATIVEVALUESPACEPROPERTY_H
#define QDECLARATIVEVALUESPACEPROPERTY_H

#include <QHash>
#include <QStringList>

#include "qvaluespace.h"
#include "qvaluespacepublisher.h"
#include "qvaluespacesubscriber.h"

QTM_USE_NAMESPACE

class QDeclarativeValueSpaceProperty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QString relativePath WRITE cd);
    Q_PROPERTY(bool server WRITE startServer);

public:
    QDeclarativeValueSpaceProperty(QObject *parent=0);

    QString path() const;
    void setPath(QString path);
    QVariant value();
    void setValue(QVariant value);

    void startServer(bool doit);

public slots:
    void cd(QString relPath);

Q_SIGNALS:
    void pathChanged();
    void changed();

    void subscribers();
    void noSubscribers();

private:
    QValueSpacePublisher *m_publisher;
    QValueSpaceSubscriber *m_subscriber;

    QStringList m_pathParts;
    QHash<QString, QValueSpacePublisher*> m_publishers;

    void makePublisher();
    void makeSubscriber();

    static bool calledStartServer;

    void connectNotify(const char *signal);

private slots:
    void onInterestChanged(QString path, bool state);

};

#endif // QDECLARATIVEVALUESPACEPUBLISHER_H
