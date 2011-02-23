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

#include "qdeclarativefileeffect_p.h"
/*!
    \qmlclass FileEffect
    \brief The FileEffect element represents feedback data stored in a file.
    \ingroup qml-feedback-api
    \inherits FeedbackEffect

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
  */
QStringList QDeclarativeFileEffect::supportedMimeTypes()
{
    return d->supportedMimeTypes();
}

/*!
    \qmlmethod  FileEffect::load()

    Makes sure that the file associated with the feedback object is loaded.
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

    makes sure that the file associated with the feedback object is unloaded.
    \sa QFeedbackFileEffect::unload()
*/
void QDeclarativeFileEffect::unload()
{
    if (isLoaded()) {
        d->unload();
        emit loadedChanged();
    }
}
