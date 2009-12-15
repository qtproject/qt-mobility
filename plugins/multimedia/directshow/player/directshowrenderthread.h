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

#ifndef DIRECTSHOWRENDERTHREAD_H
#define DIRECTSHOWRENDERTHREAD_H

#include <QtCore/qmutex.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>
#include <QtCore/qwaitcondition.h>

#include <dshow.h>

class QUrl;

class DirectShowRenderThread : public QThread
{
    Q_OBJECT
public:
    DirectShowRenderThread(QObject *parent = 0);
    ~DirectShowRenderThread();

    void render(const QUrl &url, IGraphBuilder *graph);
    void abort();

    void setAudioOutput(IBaseFilter *filter);
    void setVideoOutput(IBaseFilter *filter);

Q_SIGNALS:
    void loaded();

protected:
    void run();

private:
    void doRender(QMutexLocker *locker);
    void doSetAudioOutput(QMutexLocker *locker);
    void doSetVideoOutput(QMutexLocker *locker);

    enum State
    {
        Return = 0x01,
        Abort = 0x02,
        Render = 0x04,
        AudioOutput = 0x08,
        VideoOutput = 0x10
    };

    int m_pendingState;
    int m_executedState;
    IGraphBuilder *m_graph;
    ICaptureGraphBuilder2 *m_builder;
    IBaseFilter *m_source;
    IBaseFilter *m_audioOutput;
    IBaseFilter *m_videoOutput;
    QUrl m_url;
    QMutex m_mutex;
    QWaitCondition m_wait;
};

#endif
