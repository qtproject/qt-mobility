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

#ifndef QDECLARATIVEFILEEFFECT_H
#define QDECLARATIVEFILEEFFECT_H

#include <QtDeclarative/qdeclarative.h>
#include "qdeclarativefeedbackeffect.h"

QTM_USE_NAMESPACE

class QDeclarativeFileEffect : public QDeclarativeFeedbackEffect
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded WRITE setLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QStringList supportedMimeTypes READ supportedMimeTypes)
public:
    explicit QDeclarativeFileEffect(QObject *parent = 0) : QDeclarativeFeedbackEffect(parent){
        d = new QFeedbackFileEffect(this);
        setFeedbackEffect(d);
    }
    bool isLoaded() const
    {
        return d->isLoaded();
    }
    void setLoaded(bool v)
    {
        if (v != d->isLoaded()) {
            d->setLoaded(v);
            emit loadedChanged();
        }
    }

    QUrl source() const
    {
        return d->source();
    }
    void setSource(const QUrl & url)
    {
        if (url != d->source()) {
            d->setSource(url);
            emit sourceChanged();
        }
    }

    Q_INVOKABLE QStringList supportedMimeTypes()
    {
        return d->supportedMimeTypes();
    }


signals:
    void loadedChanged();
    void sourceChanged();
public slots:
    void load()
    {
        if (!isLoaded()) {
            d->load();
            emit loadedChanged();
        }
    }
    void unload()
    {
        if (isLoaded()) {
            d->unload();
            emit loadedChanged();
        }
    }

private:
    QFeedbackFileEffect* d;
};

QML_DECLARE_TYPE(QDeclarativeFileEffect);

#endif // QDECLARATIVEFILEEFFECT_H
