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

#include "qfeedbackplugininterfaces.h"
#include "qfeedbackplugin_p.h"
#include "qfeedbackeffect_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QHash>
#include <QDebug>

QTM_BEGIN_NAMESPACE

class FileBackend : public QFeedbackFileInterface
{
public:
    FileBackend()
    {
    }

    //this class is used to redirect the calls to all the file backends available
    virtual void setLoaded(QFeedbackFileEffect *effect, bool load)
    {
        if (load) {
            //start loading
            QFeedbackFileEffectPrivate *priv = QFeedbackFileEffectPrivate::get(effect);
            priv->isLoading = true;
            tryBackendLoad(effect);
        } else {
            //unload        
            if (QFeedbackFileInterface *subBackend = getBackend(effect))
                return subBackend->setLoaded(effect, load);
        }
    }

    virtual QFeedbackEffect::ErrorType updateEffectState(QFeedbackFileEffect *effect)
    {
        if (QFeedbackFileInterface *subBackend = getBackend(effect))
            return subBackend->updateEffectState(effect);

        return QFeedbackEffect::UnknownError;
    }

    virtual QAbstractAnimation::State actualEffectState(const QFeedbackFileEffect *effect)
    {
        if (QFeedbackFileInterface *subBackend = getBackend(effect))
            return subBackend->actualEffectState(effect);

        return QAbstractAnimation::Stopped;
    }

    virtual int effectDuration(const QFeedbackFileEffect *effect)
    {
         if (QFeedbackFileInterface *subBackend = getBackend(effect))
            return subBackend->effectDuration(effect);

        return 0;
   }

    virtual QStringList supportedMimeTypes()
    {
        QStringList ret;
        for (int i = 0; i < subBackends.count(); ++i)
            ret += subBackends.at(i)->supportedMimeTypes();
        return ret;
    }

    void addFileBackend(QFeedbackFileInterface *backend)
    {
        subBackends.append(backend);
    }

    void asyncLoadFinished(QFeedbackFileEffect *effect, bool success)
    {
        if (success) {
            //the file was loaded by the current backend
            QFeedbackFileEffectPrivate *p = QFeedbackFileEffectPrivate::get(effect);
            p->loadFinished(true);
            return;
        }

        //let's try the next backend
        tryBackendLoad(effect);
    }

private:
    QList<QFeedbackFileInterface*> subBackends; 

    QFeedbackFileInterface *getBackend(const QFeedbackFileEffect *effect)
    {
        const QFeedbackFileEffectPrivate *priv = QFeedbackFileEffectPrivate::get(effect);
        if (priv->backendUsed >= 0 && priv->backendUsed < subBackends.count())
                return subBackends.at(priv->backendUsed);
        return 0;
    }

    void tryBackendLoad(QFeedbackFileEffect *effect)
    {
        QFeedbackFileEffectPrivate *p = QFeedbackFileEffectPrivate::get(effect);
        p->backendUsed++;

        //let's try to load the file
        if (p->backendUsed >= subBackends.count()) {
            //the file couldn't be loaded
            p->loadFinished(false);
            return;
        }

        subBackends.at(p->backendUsed)->setLoaded(effect, true);
        //now we're waiting for the reply (call to asyncLoadFinished)
    }

};


struct BackendManager
{
    BackendManager() : backend(0), themeBackend(0)
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
                    backend = qobject_cast<QFeedbackHapticsInterface*>(loader.instance());
                    usedPlugin = backend != 0;
                }

                if (!themeBackend) {
                    themeBackend = qobject_cast<QFeedbackThemeInterface*>(loader.instance());
                    usedPlugin = usedPlugin || themeBackend != 0;
                }

                QFeedbackFileInterface *fileBackend = qobject_cast<QFeedbackFileInterface*>(loader.instance());
                if (fileBackend) {
                    this->fileBackend.addFileBackend(fileBackend);
                    usedPlugin = true;
                }

                if (!usedPlugin)
                    loader.unload();
            }
        }

        if (!backend)
            backend = new QDummyBackend;
    }

    QFeedbackHapticsInterface *backend;
    QFeedbackThemeInterface *themeBackend;
    FileBackend fileBackend;
};

Q_GLOBAL_STATIC(BackendManager, backendManager);

QFeedbackActuator QFeedbackHapticsInterface::createFeedbackActuator(int id)
{
    return QFeedbackActuator(id);
}

QFeedbackHapticsInterface *QFeedbackHapticsInterface::instance()
{
    return backendManager()->backend;
}

QFeedbackThemeInterface *QFeedbackThemeInterface::instance()
{
    return backendManager()->themeBackend;
}

QFeedbackFileInterface *QFeedbackFileInterface::instance()
{
    return &backendManager()->fileBackend;
}

void QFeedbackFileInterface::asyncLoadFinished(QFeedbackFileEffect *effect, bool success)
{
    backendManager()->fileBackend.asyncLoadFinished(effect, success);
}


QTM_END_NAMESPACE
