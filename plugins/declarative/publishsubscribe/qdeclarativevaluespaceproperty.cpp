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

#include "qdeclarativevaluespaceproperty.h"
#include <QCoreApplication>

bool QDeclarativeValueSpaceProperty::calledStartServer = false;

QDeclarativeValueSpaceProperty::QDeclarativeValueSpaceProperty(QObject *parent)
    : QObject(parent),
      m_publisher(NULL),
      m_subscriber(NULL)
{
}

QString QDeclarativeValueSpaceProperty::path() const
{
    return "/" + m_pathParts.join("/");
}

void QDeclarativeValueSpaceProperty::setPath(QString path)
{
    if (path.startsWith("/"))
        path = path.mid(1);
    m_pathParts = path.split("/");
    emit pathChanged();
}

void QDeclarativeValueSpaceProperty::cd(QString relPath)
{
    if (relPath.startsWith("/")) {
        setPath(relPath);
        return;
    }

    QStringList parts = relPath.split("/");
    foreach (QString part, parts) {
        if (part == "..") {
            m_pathParts.removeLast();
        } else if (part == ".") {
            // do nothing
        } else {
            m_pathParts.append(part);
        }
    }

    if (m_publisher)
        makePublisher();
    if (m_subscriber)
        makeSubscriber();

    emit pathChanged();
}

void QDeclarativeValueSpaceProperty::startServer(bool doit)
{
    if (doit && !calledStartServer) {
        QValueSpace::initValueSpaceServer();
        QCoreApplication::processEvents();
        calledStartServer = true;
    }
}

QVariant QDeclarativeValueSpaceProperty::value()
{
    if (!m_subscriber)
        makeSubscriber();

    return m_subscriber->value();
}

void QDeclarativeValueSpaceProperty::setValue(QVariant value)
{
    if (!m_publisher)
        makePublisher();

    m_publisher->setValue("", value);
}

void QDeclarativeValueSpaceProperty::makePublisher()
{    
    m_publisher = m_publishers.value(path());

    if (!m_publisher) {
        if (!calledStartServer) {
            QValueSpace::initValueSpaceServer();
            QCoreApplication::processEvents();
            calledStartServer = true;
        }

        m_publisher = new QValueSpacePublisher(path(), this);
        connect(m_publisher, SIGNAL(interestChanged(QString,bool)),
                this, SLOT(onInterestChanged(QString, bool)));
        m_publishers.insert(path(), m_publisher);
        m_publisher->resetValue("");
    }
}

void QDeclarativeValueSpaceProperty::makeSubscriber()
{
    if (m_subscriber)
        delete m_subscriber;
    m_subscriber = new QValueSpaceSubscriber(path(), this);
    connect(m_subscriber, SIGNAL(contentsChanged()),
            this, SIGNAL(changed()));
}

void QDeclarativeValueSpaceProperty::connectNotify(const char *signal)
{
    if (signal == SIGNAL(changed()) && !m_subscriber)
        makeSubscriber();
    if (signal == SIGNAL(subscribers()) && !m_publisher)
        makePublisher();
    if (signal == SIGNAL(noSubscribers()) && !m_publisher)
        makePublisher();
}

void QDeclarativeValueSpaceProperty::onInterestChanged(QString, bool state)
{
    if (state)
        emit subscribers();
    else
        emit noSubscribers();
}
