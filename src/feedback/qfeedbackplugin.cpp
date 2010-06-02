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
#include "qfeedbackplugin_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QHash>

QTM_BEGIN_NAMESPACE

static QFeedbackInterface *backend = 0;
static QThemeFeedbackInterface *themeBackend = 0;

class FileBackend : public QFileFeedbackInterface
{
public:
    //this class is used to redirect the calls to all the file backends available

    virtual void setLoaded(const QFileFeedbackEffect *effect, bool load)
    {
        if (QFileFeedbackInterface *subBackend = getBackend(effect))
            subBackend->setLoaded(effect, load);
    }

    virtual QFileFeedbackEffect::ErrorType updateEffectState(const QFileFeedbackEffect *effect)
    {
        if (QFileFeedbackInterface *subBackend = getBackend(effect))
            return subBackend->updateEffectState(effect);

        return QFileFeedbackEffect::UnknownError;
    }

    virtual QAbstractAnimation::State actualEffectState(const QFileFeedbackEffect *effect)
    {
        if (QFileFeedbackInterface *subBackend = getBackend(effect))
            return subBackend->actualEffectState(effect);

        return QAbstractAnimation::Stopped;
    }

    virtual int effectDuration(const QFileFeedbackEffect *effect)
    {
         if (QFileFeedbackInterface *subBackend = getBackend(effect))
            return subBackend->effectDuration(effect);

        return 0;
   }

    virtual QStringList supportedFileSuffixes()
    {
        return subBackends.keys();
    }

    static FileBackend *instance()
    {
        static FileBackend fileBackend;
        return &fileBackend;
    }

    void addFileBackend(QFileFeedbackInterface *backend)
    {
        QStringList exts = backend->supportedFileSuffixes();
        for (int i = 0; i < exts.count(); ++i)
            instance()->subBackends.insert(exts.at(i), backend);

    }

    QHash<QString, QFileFeedbackInterface*> subBackends; 

private:
    QFileFeedbackInterface *getBackend(const QFileFeedbackEffect *effect)
    {
        QFileInfo info = effect->file();
        return subBackends.value(info.suffix(), 0);
    }
};


static void initBackends()
{
    const QStringList paths = QCoreApplication::libraryPaths();
    for (int i = 0; i < paths.size(); ++i) {

        QDir pluginDir(paths.at(i));
        if (!pluginDir.cd(QLatin1String("feedback")))
            continue;


        foreach (const QFileInfo &pluginLib, pluginDir.entryInfoList(QDir::Files)) {
            QPluginLoader loader(pluginLib.absoluteFilePath());
            bool usedPlugin = false;

            if (!backend) {
                backend = qobject_cast<QFeedbackInterface*>(loader.instance());
                usedPlugin = backend != 0;
            }

            if (!themeBackend) {
                themeBackend = qobject_cast<QThemeFeedbackInterface*>(loader.instance());
                usedPlugin = usedPlugin || themeBackend != 0;
            }

            QFileFeedbackInterface *fileBackend = qobject_cast<QFileFeedbackInterface*>(loader.instance());
            if (fileBackend) {
                FileBackend::instance()->addFileBackend(fileBackend);
                usedPlugin = true;
            }


            if (!usedPlugin)
                loader.unload();
        }
    }

    if (!backend)
        backend = new QDummyBackend;
}

QFeedbackDevice QFeedbackInterface::createFeedbackDevice(int id)
{
    QFeedbackDevice dev;
    dev.m_id = id;
    return dev;
}

QFeedbackInterface *QFeedbackInterface::instance()
{
    if (!backend)
        initBackends();
    return backend;

}

QThemeFeedbackInterface *QThemeFeedbackInterface::instance()
{
    if (!backend)
        initBackends();
    return themeBackend;

}

QFileFeedbackInterface *QFileFeedbackInterface::instance()
{
    if (!backend)
        initBackends();
    return FileBackend::instance();
}


QTM_END_NAMESPACE
