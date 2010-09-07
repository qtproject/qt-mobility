/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef S60CAMERAVIEWFINDERENGINE_H
#define S60CAMERAVIEWFINDERENGINE_H

#include <QtCore/qobject.h>
#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>

#include <qvideosurfaceformat.h>

#include "s60cameraengineobserver.h"

class CCameraEngine;
class QAbstractVideoSurface;
#ifdef SYMBIAN_3_PLATFORM
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
#endif // SYMBIAN_3_PLATFORM

class CCameraViewfinderEngine : public QObject, public MCameraViewfinderObserver
{
    Q_OBJECT

public: // Constructor & Destructor

    CCameraViewfinderEngine(QObject *parent, CCameraEngine *engine);
    virtual ~CCameraViewfinderEngine();

public: // Methods

    // Setting Viewfinder Output
    void setVideoWidgetControl(QObject *viewfinderOutput);
    void setVideoRendererControl(QObject *viewfinderOutput);
    void setVideoWindowControl(QObject *viewfinderOutput);

    // Controls
    void startViewfinderL();
    void stopViewfinder();

protected: // MCameraViewfinderObserver

    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);

Q_SIGNALS:

    void error(int error, const QString &errorString);
    void viewFinderFrameReady(const QPixmap &pixmap);

private Q_SLOTS:

    void resetViewfinderSize(QSize size);
    void resetViewfinderDisplay();
    void viewFinderBitmapReady(const QPixmap &pixmap);

private: // Enums

    enum ViewfinderOutputType {
        OutputTypeNotSet = 0,
        OutputTypeVideoWidget,
        OutputTypeRenderer,
        OutputTypevideoWindow
    };

private: // Data

    CCameraEngine           *m_cameraEngine;
    QObject                 *m_viewfinderOutput;
    QAbstractVideoSurface   *m_viewfinderSurface; // Used only by QVideoRendererControl
#ifdef SYMBIAN_3_PLATFORM
    RWsSession              &m_wsSession;
    CWsScreenDevice         &m_screenDevice;
    RWindowBase             *m_window;
#endif // SYMBIAN_3_PLATFORM
    QSize                   m_viewfinderSize;
    ViewfinderOutputType    m_viewfinderType;
    QVideoSurfaceFormat     m_surfaceFormat; // Used only by QVideoRendererControl
};

#endif // S60CAMERAVIEWFINDERENGINE_H
