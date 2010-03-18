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

#include "sensorswindow.h"
#include "freefallform.h"
#include "accelerationform.h"
#include "orientationform.h"

SensorsWindow::SensorsWindow(QWidget* parent)
    : QMainWindow(parent), m_currentCentralWidget(NULL)
{
    setupUi(this);

    m_formNames
        << "Free fall"
        << "Acceleration"
        << "Orientation";

    QSignalMapper* signalMapper = new QSignalMapper(this);

    for (int i = 0; i < m_formNames.length(); ++i)
    {
        QAction* action = new QAction(m_formNames.at(i), this);
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(action, i);
        menuBar()->addAction(action);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(changeForm(int)));

    changeForm(0);
}

void SensorsWindow::changeForm(int formIndex)
{
    delete m_currentCentralWidget;
    m_currentCentralWidget = createForm(formIndex);
    setCentralWidget(m_currentCentralWidget);

    setWindowTitle(QString("Sensors - ") + m_formNames.at(formIndex));
}

QWidget* SensorsWindow::createForm(int formIndex)
{
    switch (formIndex)
    {
        case 0: return new FreeFallForm(this);
        case 1: return new AccelerationForm(this);
        case 2: return new OrientationForm(this);
        default: return new QWidget(this);
    }
}
