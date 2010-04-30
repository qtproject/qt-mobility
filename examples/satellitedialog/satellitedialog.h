/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOSATELLITEDIALOG_H
#define QGEOSATELLITEDIALOG_H

#include <QObject>
#include <QDialog>

#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QGeoPositionInfoSource;
class QGeoPositionInfo;
class QGeoSatelliteInfoSource;
class QGeoSatelliteInfo;
QTM_END_NAMESPACE

class SatelliteWidget;

QT_BEGIN_NAMESPACE
class QPushButton;
class QTimer;
QT_END_NAMESPACE

QTM_USE_NAMESPACE

class SatelliteDialog : public QDialog
{
    Q_OBJECT

public:
    enum ExitBehaviour {
        ExitOnFixOrCancel,
        ExitOnCancel
    };

    enum Ordering {
        OrderBySignalStrength,
        OrderByPrnNumber
    };

    enum StrengthScaling {
        ScaleToMaxAvailable,
        ScaleToMaxPossible
    };

    SatelliteDialog(QWidget *parent = 0,
                    int noSatelliteTimeoutSeconds = 30,
                    ExitBehaviour exitBehaviour = ExitOnFixOrCancel,
                    Ordering ordering = OrderBySignalStrength,
                    StrengthScaling scaling = ScaleToMaxPossible);

    void connectSources(QGeoPositionInfoSource *posSource, QGeoSatelliteInfoSource *satSource);

    ExitBehaviour exitBehaviour() const;
    void setExitBehaviour(ExitBehaviour exitBehaviour);

    Ordering ordering() const;
    void setOrdering(Ordering ordering);

    StrengthScaling strengthScaling() const;
    void setStrengthScaling(StrengthScaling scaling);

private slots:
    void positionUpdated(const QGeoPositionInfo &pos);

    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list);

    void switchButtonClicked();
    void noSatelliteTimeout();
/*#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    void restoreWindowTitle();
#endif*/

private:
    int m_noSatelliteTimeoutSeconds;
    ExitBehaviour m_exitBehaviour;
    Ordering m_ordering;
    StrengthScaling m_scaling;

    QTimer *noSatelliteTimer;

    SatelliteWidget *satelliteWidget;
    QPushButton *switchButton;
    QPushButton *cancelButton;
/*#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    QString oldTitle;
#endif*/
};

#endif // #ifndef QGEOSATELLITEDIALOG_H
