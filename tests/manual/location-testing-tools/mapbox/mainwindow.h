/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTime>

class MapBox;
class QGridLayout;
class QLayout;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QList<MapBox *> m_boxes;
    QGridLayout * m_layout;
    QLayout * m_controlLayout;
    void addBox(MapBox * box);
    QTime panTime;
    QPushButton * m_cleanButton;
    QPushButton * m_panToggle;
    QLabel * m_errorLabel;
    QTimer * m_panTimer;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void networkSessionOpened();
    void compareImages();
    void setAutoPanEnabled(bool enabled);
    void panTimerHandler();

protected:
    void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
