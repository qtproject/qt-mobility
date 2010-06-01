/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfeedbackplugin.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

QTM_BEGIN_NAMESPACE


class DummyBackend : QObject, public QFeedbackInterface
{
public:
    DummyBackend() : QObject(qApp)
    {
    }

    QFeedbackDevice defaultDevice(QFeedbackDevice::Type) { return QFeedbackDevice(); }
    QList<QFeedbackDevice> devices() { return QList<QFeedbackDevice>(); }

    QString deviceName(const QFeedbackDevice &) { return QString(); }
    QFeedbackDevice::State deviceState(const QFeedbackDevice &) { return QFeedbackDevice::Unknown; }
    QFeedbackDevice::Capabilities supportedCapabilities(const QFeedbackDevice &) { return 0; }
    QFeedbackDevice::Type type(const QFeedbackDevice &) { return QFeedbackDevice::None; }
    bool isEnabled(const QFeedbackDevice &) { return false; }
    void setEnabled(const QFeedbackDevice &, bool) { }

    QFeedbackEffect::ErrorType updateEffectProperty(const QFeedbackEffect *, EffectProperty) { return QFeedbackEffect::UnknownError; }
    QFeedbackEffect::ErrorType updateEffectState(const QFeedbackEffect *) { return QFeedbackEffect::UnknownError; }
    QAbstractAnimation::State actualEffectState(const QFeedbackEffect *) { return QAbstractAnimation::Stopped; }

    virtual void play(QFeedbackEffect::InstantEffect effect) { Q_UNUSED(effect); }

};

QFeedbackInterface *QFeedbackInterface::instance()
{
    static QFeedbackInterface *backend = 0;
    if (backend)
        return backend;

    const QStringList paths = QCoreApplication::libraryPaths();
    for (int i = 0; i < paths.size() && !backend; ++i) {

        QDir pluginDir(paths.at(i));
        if (!pluginDir.cd(QLatin1String("feedback")))
            continue;


        foreach (const QFileInfo &pluginLib, pluginDir.entryInfoList(QDir::Files)) {
            QPluginLoader loader(pluginLib.absoluteFilePath());

            backend = qobject_cast<QFeedbackInterface*>(loader.instance());
            if (backend)
                break; //we only load one plugin

            //impossible to load the plugin
            delete backend;
            loader.unload();
        }
    }

    if (!backend)
        backend = new DummyBackend;

    return backend;
}

QFeedbackDevice QFeedbackInterface::createFeedbackDevice(int id)
{
    QFeedbackDevice dev;
    dev.m_id = id;
    return dev;
}

QTM_END_NAMESPACE
