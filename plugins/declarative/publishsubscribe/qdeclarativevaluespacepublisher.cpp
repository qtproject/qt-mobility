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

#include "qdeclarativevaluespacepublisher_p.h"
#include "qdeclarativevaluespacepublishermetaobject_p.h"

#include <QDeclarativeInfo>

QDeclarativeValueSpacePublisher::QDeclarativeValueSpacePublisher(QObject *parent)
    : QObject(parent),
      d(new QDeclarativeValueSpacePublisherMetaObject(this))
{
    m_pathSet = false;
    m_hasSubscribers = false;
}

QDeclarativeValueSpacePublisher::~QDeclarativeValueSpacePublisher()
{
    if (m_pathSet)
        delete m_publisher;
}

QString QDeclarativeValueSpacePublisher::path() const
{
    return m_path;
}

void QDeclarativeValueSpacePublisher::setPath(QString path)
{
    if (m_pathSet) {
        qmlInfo(this) << "Path has already been set";
        return;
    }

    m_path = path;
    startServer(true);
    m_publisher = new QValueSpacePublisher(m_path, this);
    connect(m_publisher, SIGNAL(interestChanged(QString,bool)),
            this, SLOT(onInterestChanged(QString,bool)));
    m_pathSet = true;
}

void QDeclarativeValueSpacePublisher::setValue(const QVariant &val)
{
    if (!m_pathSet) {
        qmlInfo(this) << "Tried to set value on a publisher with no path; ignored";
        return;
    }

    m_publisher->setValue("", val);
}

void QDeclarativeValueSpacePublisher::startServer(bool really)
{
    if (really) {
        QValueSpace::initValueSpaceServer();
    }
}

bool QDeclarativeValueSpacePublisher::hasSubscribers() const
{
    return m_hasSubscribers;
}

void QDeclarativeValueSpacePublisher::setKeys(QStringList keys)
{
    foreach (QString key, keys) {
        d->addKey(key);
    }
}

void QDeclarativeValueSpacePublisher::onInterestChanged(QString path, bool state)
{
    m_hasSubscribers = state;
    // TODO: pass this on to key properties
}
