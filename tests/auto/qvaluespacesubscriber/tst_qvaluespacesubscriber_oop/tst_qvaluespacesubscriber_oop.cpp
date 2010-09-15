/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tst_qvaluespacesubscribershared.h"

#include <qvaluespace.h>

#include <QObject>
#include <QProcess>
#include <QCoreApplication>
#include <QTest>
#include <QFile>
QTM_USE_NAMESPACE

class ShutdownControl : public QObject
{
    Q_OBJECT

public:
    ShutdownControl(QProcess* process)
    {
        connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),
                this, SLOT(shutDown(int, QProcess::ExitStatus)));
    }

private slots:
    void shutDown(int, QProcess::ExitStatus)
    {
        qApp->quit();
    }

};

class tst_QValueSpaceSubscriberOop : public tst_QValueSpaceSubscriber
{
    Q_OBJECT
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

#if defined(QT_NO_PROCESS)
    tst_QValueSpaceSubscriberOop test;
    return QTest::qExec(&test, argc-1, argv);
#else
    if (args.contains("-vsClientMode")) {
        tst_QValueSpaceSubscriberOop test;
        return QTest::qExec(&test, argc-1, argv);
    } else {
#ifdef Q_OS_UNIX
        QFile::remove("/tmp/qt/valuespace_shmlayer");
#endif
        QValueSpace::initValueSpaceServer();
        QProcess process;
        ShutdownControl control(&process);
        process.setProcessChannelMode(QProcess::ForwardedChannels);
        args.removeAt(0); //don't pass the binary name
        process.start("tst_qvaluespacesubscriber_oop", args << "-vsClientMode");

        return app.exec();
    }
#endif
}

#include "tst_qvaluespacesubscriber_oop.moc"

