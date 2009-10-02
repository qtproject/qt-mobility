/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>

#include <multimedia/qmediapluginloader_p.h>
#include <multimedia/qmediaserviceproviderplugin.h>


class tst_QMediaPluginLoader : public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testKeys();
    void testInstance();
    void testInstances();

private:
    QMediaPluginLoader *loader;
};

void tst_QMediaPluginLoader::init()
{
    loader = new QMediaPluginLoader(QMediaServiceProviderFactoryInterface_iid,
                                    QLatin1String("/mediaservice"),
                                    Qt::CaseInsensitive);
}

void tst_QMediaPluginLoader::cleanup()
{
    delete loader;
}

void tst_QMediaPluginLoader::testKeys()
{
    // Each platform should have a plugin available.
    // Should work up something where if it doesn't we xfail everything
    QVERIFY(loader->keys().size() > 0);
}

void tst_QMediaPluginLoader::testInstance()
{
    foreach (const QString &key, loader->keys())
        QVERIFY(loader->instance(key) != 0);
}

void tst_QMediaPluginLoader::testInstances()
{
    foreach (const QString &key, loader->keys())
        QVERIFY(loader->instances(key).size() > 0);
}

QTEST_MAIN(tst_QMediaPluginLoader)

#include "tst_qmediapluginloader.moc"
