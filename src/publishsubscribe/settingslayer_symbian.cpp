/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "settingslayer_symbian_p.h"
#include <QVariant>
#include <QTextCodec>
#include "xqsettingskey_p.h"
#include "xqpublishandsubscribeutils.h"

#if defined(__WINS__) && !defined(SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD)
    #include "pathmapper_symbian.cpp"
    #include "qcrmlparser.cpp"
#else
    #include "pathmapper_proxy_symbian.cpp"
#endif

#include <QDebug>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(SymbianSettingsLayer, symbianSettingsLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(SymbianSettingsLayer);

const QString KeyTypeParameterRaw("raw");
const QString KeyTypeParameterString("string");

// Returns url-style parameter from path. If there is no parameter,
// null QString is returned.
// Example: Path "/cr/0x100012ab/0x1?raw" returns: "raw"
static QString getUrlParameter(const QString &path)
{
    int parameterMarkerIndex = path.lastIndexOf(QLatin1Char('?'), -1);
    if (parameterMarkerIndex >= 0)
        return path.mid(parameterMarkerIndex+1);
    else
        return QString();
}

SymbianSettingsLayer::SymbianSettingsLayer()
{
    connect(&m_settingsManager, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)),
            this, SLOT(notifyChange(const XQSettingsKey&)));
    connect(&m_settingsManager, SIGNAL(itemDeleted(const XQSettingsKey&)),
            this, SLOT(notifyChange(const XQSettingsKey&)));

    m_featureManager = 0;

    try {
        QT_TRAP_THROWING(m_featureManager = CFeatureDiscovery::NewL());
    } catch (std::exception e) {
        delete m_featureManager;
        m_featureManager = 0;
    }
}

SymbianSettingsLayer::~SymbianSettingsLayer()
{
    delete m_featureManager;
    m_featureManager = 0;

    QMutableHashIterator<QString, SymbianSettingsHandle *> i(m_handles);
    while (i.hasNext()) {
        i.next();

        removeHandle(Handle(i.value()));
    }
}

QString SymbianSettingsLayer::name()
{
    return QLatin1String("Symbian Settings Layer");
}

QUuid SymbianSettingsLayer::id()
{
    return QVALUESPACE_SYMBIAN_SETTINGS_LAYER;
}

unsigned int SymbianSettingsLayer::order()
{
    return 0;
}

QValueSpace::LayerOptions SymbianSettingsLayer::layerOptions() const
{
    return QValueSpace::PermanentLayer |
        QValueSpace::WritableLayer;
}

SymbianSettingsLayer *SymbianSettingsLayer::instance()
{
    return symbianSettingsLayer();
}

bool SymbianSettingsLayer::startup(Type type)
{
    return true;
}

bool SymbianSettingsLayer::value(Handle handle, QVariant *data)
{
    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return false;

    return value(InvalidHandle, sh->path, data);
}

bool SymbianSettingsLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    if (handle != InvalidHandle && !symbianSettingsHandle(handle))
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);
    if (handle != InvalidHandle)
        while (path.startsWith(QLatin1Char('/')))
            path = path.mid(1);
    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        handle = item(handle, path);
        createdHandle = true;
    }

    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return false;

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    QString typeParameter = getUrlParameter(fullPath);
    
    bool success = false;
    PathMapper::Target target;
    quint32 category;
    quint32 key;
    if (m_pathMapper.resolvePath(fullPath, target, category, key)) {
        if (target == PathMapper::TargetFeatureManager) {
            if (m_featureManager) {
                *data = QVariant(static_cast<bool>(
                    m_featureManager->IsSupported(key)));
                success = true;
            }
        } else {
            XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);
            QVariant readValue = m_settingsManager.readItemValue(settingsKey);
            if (readValue.type() == QVariant::ByteArray && typeParameter == KeyTypeParameterString) {
                // interpret the bytearray as utf-16 string
                QTextCodec *codec = QTextCodec::codecForName("UTF-16");
                QVariant resString;
                if (codec) {
                    resString = QVariant(codec->toUnicode(readValue.toByteArray()));
                    *data = resString;
                    success = true;
                } else {
                    success = false;
                }
            } else if (readValue.type() == QVariant::ByteArray && typeParameter != KeyTypeParameterRaw) {
                QDataStream readStream(readValue.toByteArray());
                QVariant serializedValue;
                readStream >> serializedValue;
                if (serializedValue.isValid()) {
                    *data = serializedValue;
                } else {
                    *data = readValue;
                }
                success = true;
            } else {
                *data = readValue;
                success = data->isValid();
            }
        }
    }

    if (createdHandle)
        removeHandle(handle);

    return success;
}

QSet<QString> SymbianSettingsLayer::children(Handle handle)
{
    QSet<QString> foundChildren;

    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return foundChildren;

    m_pathMapper.getChildren(sh->path, foundChildren);
    return foundChildren;
}

QAbstractValueSpaceLayer::Handle SymbianSettingsLayer::item(Handle parent, const QString &path)
{
    QString fullPath;

    // Fail on invalid path.
    if (path.isEmpty() || path.contains(QLatin1String("//")))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = path;
    } else {
        SymbianSettingsHandle *sh = symbianSettingsHandle(parent);
        if (!sh)
            return InvalidHandle;

        if (path == QLatin1String("/")) {
            fullPath = sh->path;
        } else if (sh->path.endsWith(QLatin1Char('/')) && path.startsWith(QLatin1Char('/')))
            fullPath = sh->path + path.mid(1);
        else if (!sh->path.endsWith(QLatin1Char('/')) && !path.startsWith(QLatin1Char('/')))
            fullPath = sh->path + QLatin1Char('/') + path;
        else
            fullPath = sh->path + path;
    }

    if (m_handles.contains(fullPath)) {
        SymbianSettingsHandle *sh = m_handles.value(fullPath);
        ++sh->refCount;
        return Handle(sh);
    }

    // Create a new handle for path
    SymbianSettingsHandle *sh = new SymbianSettingsHandle(fullPath);
    m_handles.insert(fullPath, sh);

    return Handle(sh);
}

void SymbianSettingsLayer::setProperty(Handle handle, Properties properties)
{
    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return;

    foreach (const QString &fullPath, m_pathMapper.childPaths(sh->path)) {
        PathMapper::Target target;
        quint32 category;
        quint32 key;
        if (m_pathMapper.resolvePath(fullPath, target, category, key) &&
            (target != PathMapper::TargetFeatureManager)) {
            XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);
            QByteArray hash;
            hash += qHash(target);
            hash += qHash((long)category);
            hash += qHash((unsigned long)key);

            if (properties & QAbstractValueSpaceLayer::Publish) {
                m_settingsManager.startMonitoring(settingsKey);
                m_monitoringHandles[hash] = sh;
                m_monitoringPaths.insert(fullPath);
            } else {
                m_settingsManager.stopMonitoring(settingsKey);
                m_monitoringHandles.remove(hash);
                m_monitoringPaths.remove(fullPath);
            }
        }
    }
}

void SymbianSettingsLayer::removeHandle(Handle handle)
{
    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return;

    if (--sh->refCount)
        return;

    m_handles.remove(sh->path);

    delete sh;
}

bool SymbianSettingsLayer::setValue(QValueSpacePublisher *creator,
                                    Handle handle,
                                    const QString &subPath,
                                    const QVariant &data)
{
    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        sh = symbianSettingsHandle(item(Handle(sh), path));
        createdHandle = true;
    }

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);
    
    QString typeParameter = getUrlParameter(fullPath);

    bool success = false;
    PathMapper::Target target;
    quint32 category;
    quint32 key;
    if (m_pathMapper.resolvePath(fullPath, target, category, key)) {
        if (target == PathMapper::TargetFeatureManager) {
            success = false;
        } else {
            if (target == PathMapper::TargetRPropery) {
                XQPublishAndSubscribeUtils utils(m_settingsManager);

                XQSettingsManager::Type type = XQSettingsManager::TypeVariant;
                if (data.type() == QVariant::Int) {
                    type = XQSettingsManager::TypeInt;
                } else {
                    type = XQSettingsManager::TypeByteArray;
                }
                utils.defineProperty(
                    XQPublishAndSubscribeSettingsKey((long)category, (unsigned long)key), type);
            }

            XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);

            if (m_monitoringPaths.contains(fullPath)) {
                m_settingsManager.startMonitoring(settingsKey);
            }

            if (data.type() == QVariant::Int || data.type() == QVariant::ByteArray ||
                    (target == PathMapper::TargetCRepository && data.type() == QVariant::Double)) {
                //Write integers and bytearrays as such (and doubles to cenrep)
                success = m_settingsManager.writeItemValue(settingsKey, data);
            } else if (data.type() == QVariant::String && typeParameter == KeyTypeParameterString) {
                // write native type string. need to convert it to utf-16, and write that into bytearray
                QByteArray stringByteArray;
                QTextCodec *codec = QTextCodec::codecForName("UTF-16");
                if (codec) {
                    stringByteArray = codec->fromUnicode(data.toString());
                    success = m_settingsManager.writeItemValue(settingsKey, QVariant(stringByteArray));
                }
            } else {
                //Write other data types serialized into a bytearray
                QByteArray byteArray;
                QDataStream writeStream(&byteArray, QIODevice::WriteOnly);
                writeStream << data;
                success = m_settingsManager.writeItemValue(settingsKey, QVariant(byteArray));
            }
        }
    }

    if (createdHandle)
        removeHandle(Handle(sh));
    return success;
}

void SymbianSettingsLayer::sync()
{
    //Not needed
}

bool SymbianSettingsLayer::removeSubTree(QValueSpacePublisher * /*creator*/, Handle /*handle*/)
{
    //Not needed
    return false;
}

bool SymbianSettingsLayer::removeValue(QValueSpacePublisher *creator,
    Handle handle,
    const QString &subPath)
{
    SymbianSettingsHandle *sh = symbianSettingsHandle(handle);
    if (!sh)
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        sh = symbianSettingsHandle(item(Handle(sh), path));
        if (!sh)
            return false;
        createdHandle = true;
    }

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    bool success = false;
    PathMapper::Target target;
    quint32 category;
    quint32 key;
    if (m_pathMapper.resolvePath(fullPath, target, category, key)) {
        if (target == PathMapper::TargetFeatureManager) {
                success = false;
        } else if (target == PathMapper::TargetRPropery) {
            XQPublishAndSubscribeUtils utils(m_settingsManager);
            utils.deleteProperty(XQPublishAndSubscribeSettingsKey((long)category, (unsigned long)key));
        }
    }

    if (createdHandle)
        removeHandle(Handle(sh));

    return success;
}

void SymbianSettingsLayer::addWatch(QValueSpacePublisher *, Handle)
{
    //Not needed
}

void SymbianSettingsLayer::removeWatches(QValueSpacePublisher *, Handle)
{
    //Not needed
}

bool SymbianSettingsLayer::supportsInterestNotification() const
{
    return false;
}

bool SymbianSettingsLayer::notifyInterest(Handle, bool)
{
    //Not needed
    return false;
}

void SymbianSettingsLayer::notifyChange(const XQSettingsKey& key)
{
    QByteArray hash;
    hash += qHash(key.target());
    hash += qHash(key.uid());
    hash += qHash(key.key());

    if (m_monitoringHandles.contains(hash)) {
        emit handleChanged(Handle(m_monitoringHandles.value(hash)));
    }
}

#include "moc_settingslayer_symbian_p.cpp"

QTM_END_NAMESPACE
