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

#include "s60camerafocuscontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraFocusControl::S60CameraFocusControl(QObject *parent)
    :QCameraFocusControl(parent)
{
    m_session = qobject_cast<S60ImageCaptureSession*>(parent);
}

S60CameraFocusControl::S60CameraFocusControl(QObject *session, QObject *parent)
   :QCameraFocusControl(parent)
   , m_session(NULL)
   , m_service(NULL)
   , m_advancedSettings(NULL)
   , m_focusLocked(false)
   , m_opticalZoomValue(1.0)
   , m_digitalZoomValue(1.0)
   , m_maxZoom(1.0)
   , m_maxDigitalZoom(1.0)
   , m_macroFocusingEnabled(false)
   , m_focusMode(QCamera::AutoFocus)
   , m_focusStatus(QCamera::FocusInitial)
   , m_error(QCamera::NoError)
   
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60ImageCaptureSession*>(session);
    connect(m_session, SIGNAL(opticalZoomChanged(qreal)), this, SIGNAL(opticalZoomChanged(qreal)));
    connect(m_session, SIGNAL(digitalZoomChanged(qreal)), this, SIGNAL(digitalZoomChanged(qreal)));
    connect(m_session, SIGNAL(focusStatusChanged(QCamera::FocusStatus)), this, SLOT(focusChanged(QCamera::FocusStatus)));
    connect(m_session, SIGNAL(advancedSettingCreated()), this, SLOT(resetAdvancedSetting()));
    m_session->setZoomFactor(m_opticalZoomValue, m_digitalZoomValue);
    m_advancedSettings = m_session->advancedSettings();    
    connect(m_advancedSettings, SIGNAL(focusStatusChanged(QCamera::FocusStatus)), this, SLOT(focusChanged(QCamera::FocusStatus)));
}

S60CameraFocusControl::~S60CameraFocusControl()
{
}

QCamera::FocusMode S60CameraFocusControl::focusMode() const
{
    return m_focusMode;
}

void S60CameraFocusControl::setFocusMode(QCamera::FocusMode mode)
{
    QCamera::FocusModes supportedModes = supportedFocusModes();
    if (supportedModes & mode)
        m_focusMode = mode;
    else
        m_focusMode = QCamera::ManualFocus; // not supported..
}

QCamera::FocusModes S60CameraFocusControl::supportedFocusModes() const
{
    if(m_advancedSettings)
        return m_advancedSettings->supportedFocusModes();
    return QCamera::FocusModes();
}

QCamera::FocusStatus S60CameraFocusControl::focusStatus() const
{
    return m_focusStatus;
}

bool S60CameraFocusControl::macroFocusingEnabled() const
{
    return m_macroFocusingEnabled;
}

bool S60CameraFocusControl::isMacroFocusingSupported() const
{
    // not supported
    return false;
}

void S60CameraFocusControl::setMacroFocusingEnabled(bool /*e*/)
{
    // not supported
    m_error = QCamera::NotSupportedFeatureError;
}

qreal S60CameraFocusControl::maximumOpticalZoom() const
{
    return m_session->maximumZoom();
}

qreal S60CameraFocusControl::maximumDigitalZoom() const
{
    return m_session->maxDigitalZoom();
}

qreal S60CameraFocusControl::opticalZoom() const
{	
	return m_session->zoomFactor();
}

qreal S60CameraFocusControl::digitalZoom() const
{   
    return m_session->digitalZoomFactor();
}

void S60CameraFocusControl::zoomTo(qreal optical, qreal digital)
{	
	m_session->setZoomFactor(optical, digital);
	m_opticalZoomValue = optical;
	m_digitalZoomValue = digital;
}

void S60CameraFocusControl::startFocusing()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        QCamera::FocusModes supportedModes = supportedFocusModes();
        if(supportedModes & m_focusMode) {
            m_advancedSettings->setFocusMode(m_focusMode);
            m_focusStatus = QCamera::FocusRequested;            
            emit focusStatusChanged(QCamera::FocusRequested);
        }
    } else {        
        emit focusStatusChanged(QCamera::FocusUnableToReach);
    }
#else
    m_session->startFocus();
    m_focusStatus = QCamera::FocusRequested;
    emit focusStatusChanged(QCamera::FocusRequested);
#endif
}

void S60CameraFocusControl::cancelFocusing()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->cancelFocusing();
        m_focusStatus = QCamera::FocusCanceled;        
        emit focusStatusChanged(QCamera::FocusCanceled);
    } else {        
        emit focusStatusChanged(QCamera::FocusUnableToReach);
    }
#else
    m_session->cancelFocus();
    m_focusStatus = QCamera::FocusCanceled;
    emit focusStatusChanged(QCamera::FocusCanceled);
#endif
}

void S60CameraFocusControl::resetAdvancedSetting()
{    
    m_advancedSettings = NULL;
    m_advancedSettings = m_session->advancedSettings();    
    connect(m_advancedSettings, SIGNAL(focusStatusChanged(QCamera::FocusStatus)), this, SLOT(focusChanged(QCamera::FocusStatus)));    
}

void S60CameraFocusControl::focusChanged(QCamera::FocusStatus status)
{
    //qDebug()<<"S60CameraFocusControl::focusChanged";
    m_focusStatus = status;
    emit focusStatusChanged(m_focusStatus);
}

QCamera::FocusPointMode S60CameraFocusControl::focusPointMode() const
{
    return QCamera::FocusPointAuto;
}

void S60CameraFocusControl::setFocusPointMode(QCamera::FocusPointMode mode)
{
    Q_UNUSED(mode);
}

QCamera::FocusPointModes S60CameraFocusControl::supportedFocusPointModes() const
{
    return QCamera::FocusPointAuto;
}

QPointF S60CameraFocusControl::customFocusPoint() const
{
    return QPointF(0.5, 0.5);
}

void S60CameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
    Q_UNUSED(point);
}

QList<QRectF> S60CameraFocusControl::focusZones() const
{
    return QList<QRectF>();
}
