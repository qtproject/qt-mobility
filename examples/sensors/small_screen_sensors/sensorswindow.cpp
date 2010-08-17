/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sensorswindow.h"
#include "freefallform.h"
#include "accelerationform.h"
#include "orientationform.h"

SensorsWindow::SensorsWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentCentralWidget(NULL)
{
    setupUi(this);

    m_formNames
        << "Free fall"
        << "Acceleration"
        << "Orientation";

    QSignalMapper *signalMapper = new QSignalMapper(this);

    for (int i = 0; i < m_formNames.length(); ++i) {
        QAction *action = new QAction(m_formNames.at(i), this);
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

QWidget *SensorsWindow::createForm(int formIndex)
{
    switch (formIndex) {
        case 0: return new FreeFallForm(this);
        case 1: return new AccelerationForm(this);
        case 2: return new OrientationForm(this);
        default: return new QWidget(this);
    }
}
