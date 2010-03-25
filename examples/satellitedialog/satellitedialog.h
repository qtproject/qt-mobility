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
