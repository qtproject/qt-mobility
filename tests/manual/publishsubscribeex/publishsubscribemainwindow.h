/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PUBLISHSUBSCRIBEMAINWINDOW_H
#define PUBLISHSUBSCRIBEMAINWINDOW_H

#include <QMainWindow>
#include <qmobilityglobal.h>

namespace Ui {
    class PublishSubscribeMainWindow;
}

QTM_BEGIN_NAMESPACE
class QValueSpaceSubscriber;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class PublishSubscribeMainWindow : public QMainWindow {
    Q_OBJECT
public:
    PublishSubscribeMainWindow(QWidget *parent = 0);
    ~PublishSubscribeMainWindow();

private slots:
    void subscriberChanged();

private:
    Ui::PublishSubscribeMainWindow *ui;
    QValueSpaceSubscriber *batterySubscriber;
    QValueSpaceSubscriber *chargingSubscriber;
    QValueSpaceSubscriber *profileSubscriber;
};

#endif // PUBLISHSUBSCRIBEMAINWINDOW_H
