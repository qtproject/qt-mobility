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

#include "qfeedbackplugininterfaces.h"
#include "qfeedbackplugin_p.h"
#include "qfeedbackeffect_p.h"
#include "qmobilitypluginsearch.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QHash>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QFeedbackInterface
    \ingroup feedback
    \inmodule QtFeedback

    \brief The QFeedbackInterface class is the base class for plugins providing feedback.

    This interface gives the possibility to report errors from within a backend plugin.
*/

/*!
    \fn QFeedbackInterface::reportError(const QFeedbackEffect *effect, QFeedbackEffect::ErrorType error)

    Allows a plugin to report the specified \a error whenever necessary.  Errors most likely can happen
    trying to play or pause an effect, which should be supplied as the parameter \a effect.
*/

/*!
    \enum QFeedbackInterface::PluginPriority

    This enum describes the priority that the plugin should have in case more than one of the same type (Haptics or Theme) is found.
    If more than one plugin has the same priority, the first one that has been loaded will be used.  However, multiple
    file effect plugins can be loaded at the same time.
    
    \value PluginLowPriority The plugin will have a low priority. This is usually the case for
    platform specific-APIs.

    \value PluginNormalPriority The plugin will have a normal priority. 
    This is usually the case for advanced technologies.

    \value PluginHighPriority The plugin will have higher priority. Use this priority if you 
    want your own plugin to be used.
*/


void QFeedbackInterface::reportError(const QFeedbackEffect *effect, QFeedbackEffect::ErrorType error)
{
    if (effect)
        emit effect->error(error);
}


// These are really useless docs, so I've marked them as internal
/*!
  \internal
  \fn QFeedbackThemeInterface::~QFeedbackThemeInterface()

  Destroys any resources used by this interface.
*/

/*!
  \internal
  \fn QFeedbackFileInterface::~QFeedbackFileInterface()

  Destroys any resources used by this interface.
*/

/*!
  \internal
  \fn QFeedbackHapticsInterface::~QFeedbackHapticsInterface()

  Destroys any resources used by this interface.
*/



template <class T>
class BackendLoader
{
public:
    BackendLoader() : inst(0) { }
    ~BackendLoader() { pl.unload(); }

    void setInstance(T *newInst) { inst = newInst; }
    T * instance() { return inst; }

    void tryLoad(QPluginLoader &loader)
    {
        if (T *newInst = qobject_cast<T*>(loader.instance())) {
            if (!inst || inst->pluginPriority() < newInst->pluginPriority()) {
                inst = newInst;
                pl.unload(); //release any reference to a previous plugin instance
                pl.setFileName(loader.fileName());
                pl.load(); //Adds a ref to the library
            }
        }
    }


private:
    QPluginLoader pl;
    T *inst;
};


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
            tryBackendLoad(effect);
        } else {
            //unload        
            if (QFeedbackFileInterface *subBackend = getBackend(effect))
                subBackend->setLoaded(effect, load);
            QFeedbackFileEffectPrivate::get(effect)->loadFinished(false); // make sure it's marked unloaded [XXX this isn't allowed to fail!]
        }
    }

    virtual void setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
    {
        if (QFeedbackFileInterface *subBackend = getBackend(effect))
            subBackend->setEffectState(effect, state);

        QFeedbackInterface::reportError(effect, QFeedbackEffect::UnknownError);
    }

    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *effect)
    {
        if (QFeedbackFileInterface *subBackend = getBackend(effect))
            return subBackend->effectState(effect);

        return QFeedbackEffect::Stopped;
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
        for (int i = 0; i < subBackends.count(); ++i) {
            ret += subBackends.at(i)->supportedMimeTypes();
        }
        return ret;
    }

    void addFileBackend(QFeedbackFileInterface *backend)
    {
        subBackends.append(backend);
    }

    void reportLoadFinished(QFeedbackFileEffect *effect, bool success)
    {
        if (success) {
            //the file was loaded by the current backend
            QFeedbackFileEffectPrivate::get(effect)->loadFinished(true);
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


class BackendManager
{
public:
    BackendManager()
    {
        const QStringList pluginPaths = mobilityPlugins(QLatin1String("feedback"));
        foreach (const QString& pluginPath, pluginPaths) {
            QPluginLoader loader(pluginPath);

            hapticsBackend.tryLoad(loader);
            themeBackend.tryLoad(loader);

            if (QFeedbackFileInterface *newFile = qobject_cast<QFeedbackFileInterface*>(loader.instance())) {
                fileBackend.addFileBackend(newFile);
            } else {
                loader.unload();
            }
        }

        if (!hapticsBackend.instance())
            hapticsBackend.setInstance(new QDummyBackend);
    }

    QFeedbackHapticsInterface* hapticsBackendInstance()
    {
        return hapticsBackend.instance();
    }

    QFeedbackThemeInterface* themeBackendInstance()
    {
        return themeBackend.instance();
    }

    FileBackend *fileBackendInstance()
    {
        return &fileBackend;
    }

private:
    BackendLoader<QFeedbackHapticsInterface> hapticsBackend;
    BackendLoader<QFeedbackThemeInterface> themeBackend;
    FileBackend fileBackend;
};

Q_GLOBAL_STATIC(BackendManager, backendManager);

/*!
    \class QFeedbackHapticsInterface
    \ingroup feedback

    \brief The QFeedbackHapticsInterface class is the base class for plugins providing custom haptics effects.

    This interface will be used to try to play custom effects with specific duration, intensity, envelope and period.
    An effect is always played on a specified actuator.
*/


/*!
   \enum QFeedbackHapticsInterface::EffectProperty
   This enum describes all effect properties for haptics effects.

   \value Duration The effect duration (in milliseconds)
   \value Intensity The effect intensity
   \value AttackTime The effect attack time (in milliseconds)
   \value AttackIntensity The effect attack intensity
   \value FadeTime The effect fade time (in milliseconds)
   \value FadeIntensity The effect fade intensity
   \value Period The effect period, this is an optional effect property.
*/

/*!
  \enum QFeedbackHapticsInterface::ActuatorProperty
  This enum describes all actuator properties.

  \value Name The actuator name.
  \value State The actuator state.
  \value Enabled The actuator enabled state.
 */


/*!
    \fn QFeedbackHapticsInterface::actuators()

    Return the available actuators provided by this plugin. The ownership of the actuator objects stays with the plugin.
*/

/*!
    \fn QFeedbackHapticsInterface::pluginPriority()

    Returns the priority for the plugin.
    \sa QFeedbackInterface::PluginPriority
*/


// XXX TODO.. these should have been pointers to QFA :/
/*!
    \fn QFeedbackHapticsInterface::setActuatorProperty(const QFeedbackActuator& actuator, ActuatorProperty property, const QVariant & value)

    Sets a \a value for \a property on the \a actuator.

    \sa ActuatorProperty
*/

/*!
    \fn QFeedbackHapticsInterface::actuatorProperty(const QFeedbackActuator & actuator, ActuatorProperty property)

    Returns the value for the \a property for an \a actuator.

    \sa ActuatorProperty
*/

/*!
    \fn QFeedbackHapticsInterface::isActuatorCapabilitySupported(const QFeedbackActuator &actuator, QFeedbackActuator::Capability capability)

    Returns true if the \a actuator supports the \a capability.
*/


/*!
    \fn QFeedbackHapticsInterface::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty property)

    Tells the backend that the \a property has been updated for the supplied \a effect.
*/

/*!
    \fn QFeedbackHapticsInterface::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)

    Sets the state to \a state for the effect \a effect. If that fails the backend should report an error by
    calling reportError and \a effect will in turn emit an error signal.
*/

/*!
    \fn QFeedbackHapticsInterface::effectState(const QFeedbackHapticsEffect *effect)

    Get the current state for the effect \a effect.
*/

/*!
    \internal
    \fn QFeedbackHapticsInterface::instance()

    Returns the instance of the object managing haptics custom effects.
    If no backend has been loaded, this will return a null pointer.
*/
QFeedbackHapticsInterface *QFeedbackHapticsInterface::instance()
{
    return backendManager()->hapticsBackendInstance();
}

/*!
    \fn QFeedbackHapticsInterface::createFeedbackActuator(QObject *parent, int id)

    Creates an instance of QFeedbackActuator with the identifier \a id and parent \a parent.  This allows
    backends to create instances of actuators. It is then up to the each backend to manage
    the identifiers according to its needs.
*/
QFeedbackActuator* QFeedbackHapticsInterface::createFeedbackActuator(QObject* parent, int id)
{
    return new QFeedbackActuator(parent, id);
}

/*!
    \class QFeedbackThemeInterface
    \ingroup feedback

    \brief The QFeedbackThemeInterface class is the base class for plugins providing themed effects.

    They can be of any nature (tactile, audio...).
    This simple interface will be used to play those themed effects by a simple call to the play method.
*/


/*!
    \fn QFeedbackThemeInterface::pluginPriority()

    Returns the priority for the plugin.
*/

/*!
    \fn QFeedbackThemeInterface::play(QFeedbackEffect::ThemeEffect effect)

    Plays the theme effect \a effect. Returns false in case of an error.
*/

/*!
    \internal
    \fn QFeedbackThemeInterface::instance()

    Returns the instance of the object managing theme effects.
    If no backend has been loaded, this will return a null pointer.
*/
QFeedbackThemeInterface *QFeedbackThemeInterface::instance()
{
    return backendManager()->themeBackendInstance();
}

/*!
    \class QFeedbackFileInterface
    \ingroup feedback

    \brief The QFeedbackFileInterface class is the base class for plugins providing support for effects stored in files.

    They can be of any nature (tactile, audio...). As it is possible to load many different file types using
    different technologies, all the backend plugins exposing this interface will be loaded at the same time.
    When loading a file all the backend will be tried in order until one can load the file. It is thus very important
    that the backends return a load status as soon as possible to not take a too long time to load a file.
*/

/*!
    \fn QFeedbackFileInterface::setLoaded(QFeedbackFileEffect* effect, bool value)

    Sets the state of the effect \a effect to be loaded if \a value is true, otherwise unloaded.
    Loading a file is asynchronous. Once the backend knows if it has loaded or can't load the file, it must
    call the reportLoadFinished function.
*/

/*!
    \fn QFeedbackFileInterface::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)

    Sets the state of \a effect to \a state.
*/

/*!
    \fn QFeedbackFileInterface::effectState(const QFeedbackFileEffect *effect)

    Returns the current state of the effect \a effect.
*/

/*!
    \fn QFeedbackFileInterface::effectDuration(const QFeedbackFileEffect *effect)

    Return the duration of \a effect, in milliseconds.
    It should return \l QFeedbackEffect::Infinite in case the duration is infinite, or 0 if undefined or unknown.
*/

/*!
    \fn QFeedbackFileInterface::supportedMimeTypes()

    Returns a list of the MIME types supported by this plugin.
*/

/*!
    \internal
    \fn QFeedbackFileInterface::instance()

    Returns the instance of the object managing theme effects.
    Even if no backend has been loaded, this will never return a null pointer.
*/
QFeedbackFileInterface *QFeedbackFileInterface::instance()
{
    return backendManager()->fileBackendInstance();
}

/*!
    \fn QFeedbackFileInterface::reportLoadFinished(QFeedbackFileEffect *effect, bool success)

    This is the function the backend should call when it has finished trying to load the effect \a effect.
    As loading a file is asynchronous and multiple plugins are attempted after each other, the
    backend has to call this function in order for the process to perform smoothly.
    The success of the operation is indicated by the \a success parameter.
*/
void QFeedbackFileInterface::reportLoadFinished(QFeedbackFileEffect *effect, bool success)
{
    backendManager()->fileBackendInstance()->reportLoadFinished(effect, success);
}

QTM_END_NAMESPACE
