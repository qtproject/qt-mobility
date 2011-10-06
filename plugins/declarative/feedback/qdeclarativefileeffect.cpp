/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativefileeffect_p.h"
/*!
    \qmlclass FileEffect
    \brief The FileEffect element represents feedback data stored in a file.
    \ingroup qml-feedback-api
    \inherits FeedbackEffect
    \since Mobility 1.1

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml File Effect

    \sa HapticsEffect, {QFeedbackActuator}
*/
QDeclarativeFileEffect::QDeclarativeFileEffect(QObject *parent)
    : QDeclarativeFeedbackEffect(parent)
{
    d = new QFeedbackFileEffect(this);
    setFeedbackEffect(d);
}

/*!
  \qmlproperty bool FileEffect::loaded

  This property is true if this feedback effect is loaded.
  \since Mobility 1.1
  */
bool QDeclarativeFileEffect::isLoaded() const
{
    return d->isLoaded();
}
void QDeclarativeFileEffect::setLoaded(bool v)
{
    if (v != d->isLoaded()) {
        d->setLoaded(v);
        emit loadedChanged();
    }
}

/*!
  \qmlproperty url FileEffect::source

  This property stores the url for the feedback data.
  \since Mobility 1.1
  */
QUrl QDeclarativeFileEffect::source() const
{
    return d->source();
}
void QDeclarativeFileEffect::setSource(const QUrl & url)
{
    if (url != d->source()) {
        d->setSource(url);
        emit sourceChanged();
    }
}

/*!
  \qmlproperty list<string> FileEffect::supportedMimeTypes

  This property holds the MIME types supported for playing effects from file.
  \since Mobility 1.1
  */
QStringList QDeclarativeFileEffect::supportedMimeTypes()
{
    return d->supportedMimeTypes();
}

/*!
    \qmlmethod  FileEffect::load()

    Makes sure that the file associated with the feedback object is loaded.
    \since Mobility 1.1
    \sa QFeedbackFileEffect::load()
*/
void QDeclarativeFileEffect::load()
{
    if (!isLoaded()) {
        d->load();
        emit loadedChanged();
    }
}

/*!
    \qmlmethod  FileEffect::unload()

    Makes sure that the file associated with the feedback object is unloaded.
    \since Mobility 1.1
    \sa QFeedbackFileEffect::unload()
*/
void QDeclarativeFileEffect::unload()
{
    if (isLoaded()) {
        d->unload();
        emit loadedChanged();
    }
}
