/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#ifndef QFEEDBACK_SYMBIAN_H
#define QFEEDBACK_SYMBIA_H

#include <qmobilityglobal.h>

#include <qfeedbackplugin.h>

#include <phonon/mediaobject.h>

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QFeedbackMediaObject;

class QFeedbackPhonon : public QObject, public QFileFeedbackInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QFileFeedbackInterface)
public:
    QFeedbackPhonon();
    virtual ~QFeedbackPhonon();

    virtual void setLoaded(QFileFeedbackEffect*, bool);
    virtual QFileFeedbackEffect::ErrorType updateEffectState(QFileFeedbackEffect *);
    virtual QAbstractAnimation::State actualEffectState(const QFileFeedbackEffect *);
    virtual int effectDuration(const QFileFeedbackEffect*);
    virtual QStringList mimeTypes();

private Q_SLOTS:
    void mediaObjectStateChanged();

private:
    QHash<const QFileFeedbackEffect *, QFeedbackMediaObject*> audioPlayers;
};

class QFeedbackMediaObject : public Phonon::MediaObject
{
    Q_OBJECT
public:
    QFeedbackMediaObject(QFeedbackPhonon *parent, QFileFeedbackEffect *effect) : Phonon::MediaObject(parent), effect(effect), overrideState(false)
    {
        connect(this, SIGNAL(stateChanged(Phonon::State, Phonon::State)), parent, SLOT(mediaObjectStateChanged())); 
    }

    QFileFeedbackEffect *effect;
    bool overrideState; //decides if the state should be picked from the effect (that's useful during a transition)
};

QT_END_HEADER

#endif
