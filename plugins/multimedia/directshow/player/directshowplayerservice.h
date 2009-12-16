/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DIRECTSHOWPLAYERSERVICE_H
#define DIRECTSHOWPLAYERSERVICE_H

#include <qmediaservice.h>

#include <QtCore/private/qwineventnotifier_p.h>

#include "directshowglobal.h"
#include "directshowrenderthread.h"

class DirectShowMetaDataControl;
class DirectShowPlayerControl;
class DirectShowVideoOutputControl;
class DirectShowVideoRendererControl;

QTM_BEGIN_NAMESPACE
class QMediaContent;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE


class DirectShowPlayerService : public QMediaService
{
    Q_OBJECT
public:
    DirectShowPlayerService(QObject *parent = 0);
    ~DirectShowPlayerService();

    QMediaControl* control(const char *name) const;

    IGraphBuilder *graph() { return m_graph; }
    IBaseFilter *source() { return 0; }

    void load(const QMediaContent &media);
    void play() { m_renderThread.play(); }
    void pause() { m_renderThread.pause(); }
    void stop() { m_renderThread.stop(); }

    void seek(qint64 position) { m_renderThread.seek(position); }
    void setRate(qreal rate) { m_renderThread.setRate(rate); }


private Q_SLOTS:
    void videoOutputChanged();
    void graphEvent(HANDLE handle);
    void loaded();

private:
    DirectShowPlayerControl *m_playerControl;
    DirectShowMetaDataControl *m_metaDataControl;
    DirectShowVideoOutputControl *m_videoOutputControl;
    DirectShowVideoRendererControl *m_videoRendererControl;
    IGraphBuilder *m_graph;
    DirectShowRenderThread m_renderThread;
    QWinEventNotifier m_graphEventNotifier;
};


#endif
