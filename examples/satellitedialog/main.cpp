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

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>

#include "satellitedialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SatelliteDialog *dialog = new SatelliteDialog(0,
            30,
            SatelliteDialog::ExitOnCancel,
            SatelliteDialog::OrderByPrnNumber,
            SatelliteDialog::ScaleToMaxPossible);

    QGeoPositionInfoSource *posSource = QGeoPositionInfoSource::createDefaultSource(0);
    QGeoSatelliteInfoSource *satSource = QGeoSatelliteInfoSource::createDefaultSource(0);

    if ((posSource == 0) || (satSource == 0)) {
        QMessageBox::critical(0, "SatelliteDialog", "This examples requires a valid location source and no valid location sources are available on this platform.");
        return -1;
    }

    posSource->setUpdateInterval(5000);

    dialog->connectSources(posSource, satSource);

    posSource->startUpdates();
    satSource->startUpdates();

    QTimer::singleShot(1, dialog, SLOT(show()));

    int result = app.exec();

    posSource->stopUpdates();
    satSource->stopUpdates();

    delete posSource;
    delete satSource;
    delete dialog;

    return result;
}
