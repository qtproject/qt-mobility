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

#include "qvaluespace_p.h"
#include "qvaluespacepublisher.h"

#include <QSet>
#include <QStringList>
#include <QEventLoop>
#include <QTimer>
#include <QVariant>
#include <QMutex>

#ifdef Q_OS_WINCE
#include <QThread>
#endif

#include <QDebug>

// Define win32 version to pull in RegisterWaitForSingleObject and UnregisterWait.
#define _WIN32_WINNT 0x0500
#include <windows.h>

#ifdef Q_OS_WINCE
#include <regext.h>
#define RegistryCallback REGISTRYNOTIFYCALLBACK
#else
#define RegistryCallback WAITORTIMERCALLBACK
#endif

QTM_BEGIN_NAMESPACE

#ifdef Q_OS_WINCE
class QWindowsCENotify : public QThread
{
    Q_OBJECT

public:
    QWindowsCENotify(QObject *parent = 0);
    ~QWindowsCENotify();

    void addHandle(HANDLE handle);
    void removeHandle(HANDLE handle);

protected:
    void run();

signals:
    void eventSignaled(void *handle);

private:
    HANDLE wakeUp;
    QSet<HANDLE> handles;
};

QWindowsCENotify::QWindowsCENotify(QObject *parent)
:   QThread(parent)
{
    wakeUp = CreateEvent(0, false, false, 0);
}

QWindowsCENotify::~QWindowsCENotify()
{
    CloseHandle(wakeUp);
    wakeUp = INVALID_HANDLE_VALUE;
    wait();
}

void QWindowsCENotify::addHandle(HANDLE handle)
{
    handles.insert(handle);

    SetEvent(wakeUp);
}

void QWindowsCENotify::removeHandle(HANDLE handle)
{
    handles.remove(handle);

    SetEvent(wakeUp);
}

void QWindowsCENotify::run()
{
    while (wakeUp != INVALID_HANDLE_VALUE) {
        ResetEvent(wakeUp);

        QVector<HANDLE> waitHandles = handles.toList().toVector();
        waitHandles.append(wakeUp);
        DWORD index = WaitForMultipleObjects(waitHandles.count(), waitHandles.data(),
                                             false, INFINITE);

        if (index >= WAIT_OBJECT_0 && index < WAIT_OBJECT_0 + waitHandles.count()) {
            if (index - WAIT_OBJECT_0 != waitHandles.count() - 1) {
                handles.remove(waitHandles.at(index - WAIT_OBJECT_0));
                emit eventSignaled(waitHandles.at(index - WAIT_OBJECT_0));
            }
        } else if (index >= WAIT_ABANDONED_0 && index < WAIT_ABANDONED_0 + waitHandles.count()) {
            //qDebug() << "woke up because of abandoned index" << index - WAIT_ABANDONED_0;
        } else {
            qDebug() << "WaitForMultipleObjects failed with error" << GetLastError();
        }
    }
}
#endif

class RegistryLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    RegistryLayer(const QString &basePath, bool volatileKeys, RegistryCallback callback);
    ~RegistryLayer();

    /* Common functions */
    bool startup(Type t);

    Handle item(Handle parent, const QString &path);
    void removeHandle(Handle handle);
    void setProperty(Handle handle, Properties);

    bool value(Handle handle, QVariant *data);
    bool value(Handle handle, const QString &subPath, QVariant *data);
    QSet<QString> children(Handle handle);

    /* QValueSpaceSubscriber functions */
    bool supportsInterestNotification() const;
    bool notifyInterest(Handle handle, bool interested);

    /* QValueSpacePublisher functions */
    bool setValue(QValueSpacePublisher *creator, Handle handle, const QVariant &data);
    bool setValue(QValueSpacePublisher *creator, Handle handle, const QString &path,
                  const QVariant &data);
    bool removeValue(QValueSpacePublisher *creator, Handle handle, const QString &subPath);
    bool removeSubTree(QValueSpacePublisher *creator, Handle parent);
    void addWatch(QValueSpacePublisher *creator, Handle handle);
    void removeWatches(QValueSpacePublisher *creator, Handle parent);
    void sync();

public slots:
    void emitHandleChanged(void *hkey);
#ifdef Q_OS_WINCE
    void eventSignaled(void *handle);
#endif

private:
    struct RegistryHandle {
        RegistryHandle(const QString &p)
        :   path(p), valueHandle(false), refCount(1)
        {
        }

        QString path;
        bool valueHandle;
        unsigned int refCount;
    };

    void openRegistryKey(RegistryHandle *handle);
    bool createRegistryKey(RegistryHandle *handle);
    bool removeRegistryValue(RegistryHandle *handle, const QString &path);
    void closeRegistryKey(RegistryHandle *handle);
    void pruneEmptyKeys(RegistryHandle *handle);

    QMutex localLock;
    QString m_basePath;
    bool m_volatileKeys;
    RegistryCallback m_callback;

    QHash<QString, RegistryHandle *> handles;

    RegistryHandle *registryHandle(Handle handle)
    {
        if (handle == InvalidHandle)
            return 0;

        RegistryHandle *h = reinterpret_cast<RegistryHandle *>(handle);
        if (handles.values().contains(h))
            return h;

        return 0;
    }

    QMap<RegistryHandle *, HKEY> hKeys;
    QMultiMap<RegistryHandle *, RegistryHandle *> notifyProxies;
    // MinGW complains about QPair<::HANDLE, ::HANDLE>
    typedef ::HANDLE HandleType;
    typedef QPair<HandleType, HandleType> HandlePair;
    QMap<HKEY, HandlePair > waitHandles;
#ifdef Q_OS_WINCE
    QWindowsCENotify *notifyThread;
#endif

    QMap<QValueSpacePublisher *, QList<QString> > creators;
};

class VolatileRegistryLayer : public RegistryLayer
{
    Q_OBJECT

public:
    VolatileRegistryLayer();
    ~VolatileRegistryLayer();

    /* Common functions */
    QString name();

    QUuid id();
    unsigned int order();

    QValueSpace::LayerOptions layerOptions() const;

    static VolatileRegistryLayer *instance();
};

Q_GLOBAL_STATIC(VolatileRegistryLayer, volatileRegistryLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(VolatileRegistryLayer);

class NonVolatileRegistryLayer : public RegistryLayer
{
    Q_OBJECT

public:
    NonVolatileRegistryLayer();
    ~NonVolatileRegistryLayer();

    /* Common functions */
    QString name();

    QUuid id();
    unsigned int order();

    QValueSpace::LayerOptions layerOptions() const;

    static NonVolatileRegistryLayer *instance();
};

Q_GLOBAL_STATIC(NonVolatileRegistryLayer, nonVolatileRegistryLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(NonVolatileRegistryLayer);

#ifdef Q_OS_WINCE
void qVolatileRegistryLayerCallback(HREGNOTIFY hNotify, DWORD dwUserData,
                                    const PBYTE pData, const UINT cbdata)
{
}

void qNonVolatileRegistryLayerCallback(HREGNOTIFY hNotify, DWORD dwUserData,
                                    const PBYTE pData, const UINT cbdata)
{
}

#else
void CALLBACK qVolatileRegistryLayerCallback(PVOID lpParameter, BOOLEAN)
{
    QMetaObject::invokeMethod(volatileRegistryLayer(), "emitHandleChanged", Qt::QueuedConnection,
                              Q_ARG(void *, lpParameter));
}

void CALLBACK qNonVolatileRegistryLayerCallback(PVOID lpParameter, BOOLEAN)
{
    QMetaObject::invokeMethod(nonVolatileRegistryLayer(), "emitHandleChanged",
                              Qt::QueuedConnection, Q_ARG(void *, lpParameter));
}
#endif

static QString qConvertPath(const QString &path)
{
    QString fixedPath(path);

    while (fixedPath.endsWith(QLatin1Char('/')))
        fixedPath.chop(1);

    fixedPath.replace(QLatin1Char('/'), QLatin1Char('\\'));

    return fixedPath;
}

VolatileRegistryLayer::VolatileRegistryLayer()
:   RegistryLayer(QLatin1String("Software/Nokia/QtMobility/volatileContext"), true,
                  &qVolatileRegistryLayerCallback)
{
}

VolatileRegistryLayer::~VolatileRegistryLayer()
{
}

QString VolatileRegistryLayer::name()
{
    return QLatin1String("Volatile Registry Layer");
}

QUuid VolatileRegistryLayer::id()
{
    return QVALUESPACE_VOLATILEREGISTRY_LAYER;
}

unsigned int VolatileRegistryLayer::order()
{
    return 0x1000;
}

QValueSpace::LayerOptions VolatileRegistryLayer::layerOptions() const
{
    return QValueSpace::TransientLayer | QValueSpace::WritableLayer;
}

VolatileRegistryLayer *VolatileRegistryLayer::instance()
{
    return volatileRegistryLayer();
}

NonVolatileRegistryLayer::NonVolatileRegistryLayer()
:   RegistryLayer(QLatin1String("Software/Nokia/QtMobility/nonVolatileContext"), false,
                  &qNonVolatileRegistryLayerCallback)
{
}

NonVolatileRegistryLayer::~NonVolatileRegistryLayer()
{
}

QString NonVolatileRegistryLayer::name()
{
    return QLatin1String("Non-Volatile Registry Layer");
}

QUuid NonVolatileRegistryLayer::id()
{
    return QVALUESPACE_NONVOLATILEREGISTRY_LAYER;
}

unsigned int NonVolatileRegistryLayer::order()
{
    return 0;
}

QValueSpace::LayerOptions NonVolatileRegistryLayer::layerOptions() const
{
    return QValueSpace::PermanentLayer | QValueSpace::WritableLayer;
}

NonVolatileRegistryLayer *NonVolatileRegistryLayer::instance()
{
    return nonVolatileRegistryLayer();
}

RegistryLayer::RegistryLayer(const QString &basePath, bool volatileKeys, RegistryCallback callback)
:   localLock(QMutex::Recursive), m_basePath(basePath), m_volatileKeys(volatileKeys),
    m_callback(callback)
{
    // Ensure that the m_basePath key exists and is non-volatile.
    HKEY key;
    RegCreateKeyEx(HKEY_CURRENT_USER,
                    reinterpret_cast<const wchar_t*>(qConvertPath(m_basePath).utf16()),
                    0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, 0);

    RegCloseKey(key);

#ifdef Q_OS_WINCE
    notifyThread = new QWindowsCENotify(this);
    connect(notifyThread, SIGNAL(eventSignaled(void*)),
            this, SLOT(eventSignaled(void*)), Qt::QueuedConnection);
    notifyThread->start();
#endif
}

RegistryLayer::~RegistryLayer()
{
    QMutableHashIterator<QString, RegistryHandle *> i(handles);
    while (i.hasNext()) {
        i.next();

        RegistryHandle *handle = i.value();

        if (handle->valueHandle)
            removeHandle(Handle(handle));
    }

    i.toFront();
    while (i.hasNext()) {
        i.next();

        removeHandle(Handle(i.value()));
    }
}

bool RegistryLayer::startup(Type)
{
    return true;
}

bool RegistryLayer::value(Handle handle, QVariant *data)
{
    QMutexLocker locker(&localLock);

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return false;

    return value(InvalidHandle, rh->path, data);
}

bool RegistryLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    QMutexLocker locker(&localLock);

    if (handle != InvalidHandle && !registryHandle(handle))
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

    RegistryHandle *rh = registryHandle(handle);

    openRegistryKey(rh);
    if (!hKeys.contains(rh)) {
        if (createdHandle)
            removeHandle(handle);

        return false;
    }

    HKEY key = hKeys.value(rh);

    DWORD regSize = 0;
    long result = RegQueryValueEx(key,
            reinterpret_cast<const wchar_t*>(value.utf16()), 0, 0, 0, &regSize);
    if (result == ERROR_FILE_NOT_FOUND) {
        *data = QVariant();
        if (createdHandle)
            removeHandle(handle);
        return false;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegQueryValueEx failed with error" << result;
        *data = QVariant();
        if (createdHandle)
            removeHandle(handle);
        return false;
    }

    BYTE *regData = new BYTE[regSize];
    DWORD regType;

    result = RegQueryValueEx(key,
            reinterpret_cast<const wchar_t*>(value.utf16()), 
            0, &regType, regData, &regSize);
    if (result != ERROR_SUCCESS) {
        qDebug() << "real RegQueryValueEx failed with error" << result;
        if (createdHandle)
            removeHandle(handle);
        return false;
    }

    switch (regType) {
    case REG_DWORD:
        *data = qVariantFromValue(*reinterpret_cast<uint *>(regData));
        break;
    case REG_QWORD:
        *data = qVariantFromValue(*reinterpret_cast<quint64 *>(regData));
        break;
    case REG_SZ:
        *data = qVariantFromValue(QString::fromWCharArray(reinterpret_cast<wchar_t *>(regData)));
        break;
    case REG_MULTI_SZ: {
        QStringList list;
        wchar_t *temp = reinterpret_cast<wchar_t *>(regData);
        while (*temp != L'\0') {
            QString string = QString::fromWCharArray(temp);
            list << string;
            temp += string.length() + 1;
        }
        *data = qVariantFromValue(list);
        break;
    }
    case REG_BINARY:
        *data = qVariantFromValue(QByteArray(reinterpret_cast<char *>(regData), regSize));
        break;
    case REG_NONE: {
        QDataStream stream(QByteArray::fromRawData(reinterpret_cast<char *>(regData), regSize));
        stream >> *data;
        break;
    }
    default:
        qDebug() << "Unknown REG type" << regType;
        delete[] regData;
        if (createdHandle)
            removeHandle(handle);
        return false;
        break;
    }

    delete[] regData;

    if (createdHandle)
        removeHandle(handle);

    return true;
}

#define MAX_KEY_LENGTH 255
#define MAX_NAME_LENGTH 16383
QSet<QString> RegistryLayer::children(Handle handle)
{
    QMutexLocker locker(&localLock);

    QSet<QString> foundChildren;

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return foundChildren;

    openRegistryKey(rh);
    if (rh->valueHandle || !hKeys.contains(rh))
        return foundChildren;

    HKEY key = hKeys.value(rh);
    int i = 0;
    long result;
    do {
        TCHAR subKey[MAX_KEY_LENGTH];
        DWORD subKeySize = MAX_KEY_LENGTH;

        result = RegEnumKeyEx(key, i, subKey, &subKeySize, 0, 0, 0, 0);
        if (result == ERROR_KEY_DELETED) {
            QMetaObject::invokeMethod(this, "emitHandleChanged", Qt::QueuedConnection,
                                      Q_ARG(void *, key));
            break;
        }
        if (result != ERROR_SUCCESS)
            break;

        foundChildren << QString::fromWCharArray(subKey, subKeySize);
        ++i;
    } while (result == ERROR_SUCCESS);

    i = 0;
    do {
        TCHAR valueName[MAX_NAME_LENGTH];
        DWORD valueNameSize = MAX_NAME_LENGTH;

        result = RegEnumValue(key, i, valueName, &valueNameSize, 0, 0, 0, 0);
        if (result == ERROR_KEY_DELETED) {
            QMetaObject::invokeMethod(this, "emitHandleChanged", Qt::QueuedConnection,
                                      Q_ARG(void *, key));
            break;
        }
        if (result != ERROR_SUCCESS)
            break;

        foundChildren << QString::fromWCharArray(valueName, valueNameSize);
        ++i;
    } while (result == ERROR_SUCCESS);

    return foundChildren;
}

QAbstractValueSpaceLayer::Handle RegistryLayer::item(Handle parent, const QString &path)
{
    QMutexLocker locker(&localLock);

    QString fullPath;

    // Fail on invalid path.
    if (path.isEmpty() || path.contains(QLatin1String("//")))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = path;
    } else {
        RegistryHandle *rh = registryHandle(parent);
        if (!rh)
            return InvalidHandle;

        if (path == QLatin1String("/")) {
            fullPath = rh->path;
        } else if (rh->path.endsWith(QLatin1Char('/')) && path.startsWith(QLatin1Char('/')))
            fullPath = rh->path + path.mid(1);
        else if (!rh->path.endsWith(QLatin1Char('/')) && !path.startsWith(QLatin1Char('/')))
            fullPath = rh->path + QLatin1Char('/') + path;
        else
            fullPath = rh->path + path;
    }

    if (handles.contains(fullPath)) {
        RegistryHandle *rh = handles.value(fullPath);
        ++rh->refCount;
        return Handle(rh);
    }

    // Create a new handle for path
    RegistryHandle *rh = new RegistryHandle(fullPath);
    handles.insert(fullPath, rh);

    return Handle(rh);
}

void RegistryLayer::setProperty(Handle handle, Properties properties)
{
    QMutexLocker locker(&localLock);

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return;

    if (properties & QAbstractValueSpaceLayer::Publish) {
        // Enable change notification for handle
        openRegistryKey(rh);
        if (!hKeys.contains(rh)) {
            // The key does not exist. Temporarily use the parent key as a proxy.
            QString path = rh->path;
            while (!path.isEmpty()) {
                rh = registryHandle(item(InvalidHandle, path));
                openRegistryKey(rh);
                if (hKeys.contains(rh)) {
                    notifyProxies.insert(rh, registryHandle(handle));
                    break;
                }
                removeHandle(Handle(rh));

                // root path doesn't exists
                if (path.length() == 1)
                    return;

                int index = path.lastIndexOf(QLatin1Char('/'));
                if (index == 0)
                    path.truncate(1);
                else
                    path.truncate(index);
            }
        }

        HKEY key = hKeys.value(rh);

        if (waitHandles.contains(key))
            return;

#ifdef Q_OS_WINCE
        DWORD filter = REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET;
        ::HANDLE event = CeFindFirstRegChange(key, true, filter);
        if (event == INVALID_HANDLE_VALUE) {
            qDebug() << "CeFindFirstRegChange failed with error" << GetLastError();
            return;
        }

        notifyThread->addHandle(event);

        waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, INVALID_HANDLE_VALUE));
#else
        ::HANDLE event = CreateEvent(0, false, false, 0);
        if (event == 0) {
            qDebug() << "CreateEvent failed with error" << GetLastError();
            return;
        }

        DWORD filter = REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_LAST_SET;
        long result = RegNotifyChangeKeyValue(key, true, filter, event, true);

        if (result != ERROR_SUCCESS) {
            qDebug() << "RegNotifyChangeKeyValue failed with error" << result;
            return;
        }

        ::HANDLE waitHandle;
        if (!RegisterWaitForSingleObject(&waitHandle, event, m_callback, key,
                                         INFINITE, WT_EXECUTEDEFAULT)) {
            qDebug() << "RegisterWaitForSingleObject failed with error" << GetLastError();
            return;
        }

        //waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
        waitHandles.insert(key, HandlePair(event, waitHandle));
#endif
    }
    if (!(properties & QAbstractValueSpaceLayer::Publish)) {
        // Disable change notification for handle
        if (!hKeys.contains(rh))
            return;

        HKEY key = hKeys.value(rh);

        if (waitHandles.contains(key)) {
            //QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
            HandlePair wait = waitHandles.take(key);

#ifdef Q_OS_WINCE
            notifyThread->removeHandle(wait.first);
#else
            UnregisterWait(wait.second);
#endif

            CloseHandle(wait.first);
        }
    }
}

void RegistryLayer::removeHandle(Handle handle)
{
    QMutexLocker locker(&localLock);

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return;

    if (--rh->refCount)
        return;

    QList<RegistryHandle *> proxies = notifyProxies.keys(rh);
    while (!proxies.isEmpty()) {
        notifyProxies.remove(proxies.first(), rh);
        removeHandle(Handle(proxies.takeFirst()));
    }

    handles.remove(rh->path);

    closeRegistryKey(rh);

    delete rh;
}

void RegistryLayer::closeRegistryKey(RegistryHandle *handle)
{
    QMutexLocker locker(&localLock);

    if (!hKeys.contains(handle))
        return;

    HKEY key = hKeys.take(handle);

    // Check if other handles are using this registry key.
    if (!hKeys.keys(key).isEmpty())
        return;

    if (waitHandles.contains(key)) {
        //QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
        HandlePair wait = waitHandles.take(key);

#ifdef Q_OS_WINCE
        notifyThread->removeHandle(wait.first);
#else
        UnregisterWait(wait.second);
#endif

        CloseHandle(wait.first);
    }

    RegCloseKey(key);
}

static LONG qRegDeleteTree(HKEY hKey, LPCTSTR lpSubKey)
{
    HKEY key;
    long result = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &key);
    if (result != ERROR_SUCCESS) {
        if (result == ERROR_FILE_NOT_FOUND)
            return ERROR_SUCCESS;
        else
            return result;
    }

    do {
        TCHAR subKey[MAX_KEY_LENGTH];
        DWORD subKeySize = MAX_KEY_LENGTH;

        result = RegEnumKeyEx(key, 0, subKey, &subKeySize, 0, 0, 0, 0);
        if (result == ERROR_NO_MORE_ITEMS)
            break;

        result = qRegDeleteTree(key, subKey);
    } while (result == ERROR_SUCCESS);

    RegCloseKey(key);

    if (result != ERROR_NO_MORE_ITEMS && result != ERROR_SUCCESS)
        return result;

    return RegDeleteKey(hKey, lpSubKey);
}

bool RegistryLayer::removeRegistryValue(RegistryHandle *handle, const QString &subPath)
{
    QMutexLocker locker(&localLock);

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    RegistryHandle *rh;
    QString value;
    if (index == -1) {
        rh = handle;
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);
        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        rh = registryHandle(item(handle ? Handle(handle) : InvalidHandle, path));

        createdHandle = true;
    }

#ifdef Q_OS_WINCE
    QList<RegistryHandle *> deletedKeys;
    QString fullPath;
    if (rh && rh->path != QLatin1String("/"))
        fullPath = rh->path + QLatin1Char('/') + value;
    else
        fullPath = QLatin1Char('/') + value;

    foreach (RegistryHandle *h, hKeys.keys()) {
        if (h->path.startsWith(fullPath) && !h->valueHandle) {
            deletedKeys.append(h);
            closeRegistryKey(h);
        }
    }
#endif

    openRegistryKey(rh);
    if (!hKeys.contains(rh)) {
        if (createdHandle)
            removeHandle(Handle(rh));

        return false;
    }

    HKEY key = hKeys.value(rh);

    long result = RegDeleteValue(key, reinterpret_cast<const wchar_t*>(value.utf16()));
    if (result == ERROR_FILE_NOT_FOUND) {
        result = qRegDeleteTree(key, reinterpret_cast<const wchar_t*>(value.utf16()));
        if (result == ERROR_SUCCESS) {
            const QString rootPath = rh->path;

            QList<QString> paths = handles.keys();
            while (!paths.isEmpty()) {
                QString p = paths.takeFirst();

                if (p.startsWith(rootPath))
                    closeRegistryKey(handles.value(p));
            }
        }
        if (result != ERROR_SUCCESS)
            qDebug() << "RegDeleteTree failed with error" << result;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegDeleteValue failed with error" << result;
    }

#ifdef Q_OS_WINCE
    while (!deletedKeys.isEmpty())
        emit handleChanged(Handle(deletedKeys.takeFirst()));
#endif

    if (createdHandle)
        removeHandle(Handle(rh));

    return result == ERROR_SUCCESS;
}

bool RegistryLayer::setValue(QValueSpacePublisher *creator, Handle handle, const QVariant &data)
{
    return setValue(creator, handle, QString(), data);
}

bool RegistryLayer::setValue(QValueSpacePublisher *creator, Handle handle, const QString &subPath,
                             const QVariant &data)
{
    QMutexLocker locker(&localLock);

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
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

        rh = registryHandle(item(Handle(rh), path));
        createdHandle = true;
    }

    if (createRegistryKey(rh)) {
        if (!creators[creator].contains(rh->path))
            creators[creator].append(rh->path);
    }
    if (!hKeys.contains(rh)) {
        if (createdHandle)
            removeHandle(Handle(rh));

        return false;
    }

    HKEY key = hKeys.value(rh);

    long result;

    switch (data.type()) {
    case QVariant::UInt: {
        DWORD temp = data.toUInt();
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                               REG_DWORD, reinterpret_cast<const BYTE *>(&temp), sizeof(DWORD));
        break;
    }
    case QVariant::ULongLong: {
        quint64 temp = data.toULongLong();
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                               REG_QWORD, reinterpret_cast<const BYTE *>(&temp), sizeof(quint64));
        break;
    }
    case QVariant::String: {
        // This may be wrong!
        QString tempString = data.toString();
        int length = tempString.length() + 1;
        wchar_t *temp = new wchar_t[length];
        tempString.toWCharArray(temp);
        temp[length - 1] = L'\0';
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                    REG_SZ, reinterpret_cast<const BYTE *>(temp), length * sizeof(wchar_t));
        delete[] temp;
        break;
    }
    case QVariant::StringList: {
        const QString joined = data.toStringList().join(QString(QLatin1Char('\0')));
        int length = joined.length() + 2;
        wchar_t *temp = new wchar_t[length];
        joined.toWCharArray(temp);
        temp[length - 2] = L'\0';
        temp[length - 1] = L'\0';
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                               REG_MULTI_SZ, reinterpret_cast<const BYTE *>(temp),
                               length * sizeof(wchar_t));
        delete[] temp;
        break;
    }
    case QVariant::ByteArray: {
        QByteArray temp = data.toByteArray();
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                               REG_BINARY, reinterpret_cast<const BYTE *>(temp.constData()),
                               temp.length());
        break;
    }
    default: {
        QByteArray temp;
        QDataStream stream(&temp, QIODevice::WriteOnly | QIODevice::Truncate);
        stream << data;
        result = RegSetValueEx(key, reinterpret_cast<const wchar_t*>(value.utf16()), 0,
                               REG_NONE, reinterpret_cast<const BYTE *>(temp.constData()),
                               temp.length());
        break;
    }
    };

    QString fullPath(rh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    if (!creators[creator].contains(fullPath))
        creators[creator].append(fullPath);

    if (createdHandle)
        removeHandle(Handle(rh));

    return result == ERROR_SUCCESS;
}

void RegistryLayer::sync()
{
    QMutexLocker locker(&localLock);

    // Wait for change notification callbacks before returning
    QEventLoop loop;
    connect(this, SIGNAL(handleChanged(quintptr)), &loop, SLOT(quit()));
    bool wait = false;

    QList<HKEY> keys = hKeys.values();
    while (!keys.isEmpty()) {
        HKEY key = keys.takeFirst();

        if (!wait && waitHandles.contains(key))
            wait = true;

        RegFlushKey(key);
    }

    if (wait) {
        locker.unlock();
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
    }
}

void RegistryLayer::emitHandleChanged(void *k)
{
    QMutexLocker locker(&localLock);

    HKEY key = reinterpret_cast<HKEY>(k);

    QList<RegistryHandle *> changedHandles = hKeys.keys(key);
    if (changedHandles.isEmpty()) {
        if (waitHandles.contains(key)) {
            //QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
            HandlePair wait = waitHandles.take(key);

#ifdef Q_OS_WINCE
            notifyThread->removeHandle(wait.first);
#else
            UnregisterWait(wait.second);
#endif
            CloseHandle(wait.first);
            return;
        }
    }

    while (!changedHandles.isEmpty()) {
        RegistryHandle *handle = changedHandles.takeFirst();
        emit handleChanged(Handle(handle));

        // Emit signal for handles that this handle is proxying for.
        foreach (RegistryHandle *proxied, notifyProxies.values(handle)) {
            openRegistryKey(proxied);
            if (hKeys.contains(proxied)) {
                notifyProxies.remove(handle, proxied);
                removeHandle(Handle(handle));
                setProperty(Handle(proxied), Publish);

                emit handleChanged(Handle(proxied));
            }
        }
    }

    if (waitHandles.contains(key)) {
        //QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
        HandlePair wait = waitHandles.take(key);

        ::HANDLE event = wait.first;

#ifdef Q_OS_WINCE
        notifyThread->removeHandle(event);
#else
        UnregisterWait(wait.second);
#endif

#ifdef Q_OS_WINCE
        if (!CeFindNextRegChange(event)) {
            long result = GetLastError();
            if (result == ERROR_KEY_DELETED) {
                CloseHandle(event);

                QList<RegistryHandle *> changedHandles = hKeys.keys(key);

                for (int i = 0; i < changedHandles.count(); ++i)
                    hKeys.remove(changedHandles.at(i));

                RegCloseKey(key);

                for (int i = 0; i < changedHandles.count(); ++i)
                    setProperty(Handle(changedHandles.at(i)), Publish);
            } else {
                qDebug() << "CeFindNextRegChange failed with error" << result;
            }

            return;
        }

        notifyThread->addHandle(event);

        waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, INVALID_HANDLE_VALUE));
#else
        long result = RegNotifyChangeKeyValue(key, true, REG_NOTIFY_CHANGE_NAME |
                                              REG_NOTIFY_CHANGE_ATTRIBUTES |
                                              REG_NOTIFY_CHANGE_LAST_SET,
                                              event, true);

        if (result != ERROR_SUCCESS) {
            if (result == ERROR_KEY_DELETED || result == ERROR_INVALID_PARAMETER) {
                CloseHandle(event);

                QList<RegistryHandle *> changedHandles = hKeys.keys(key);

                for (int i = 0; i < changedHandles.count(); ++i)
                    hKeys.remove(changedHandles.at(i));

                RegCloseKey(key);

                for (int i = 0; i < changedHandles.count(); ++i)
                    setProperty(Handle(changedHandles.at(i)), Publish);
            } else {
                qDebug() << "RegNotifyChangeKeyValue failed with error" << result;
            }

            return;
        }

        ::HANDLE waitHandle;

        if (!RegisterWaitForSingleObject(&waitHandle, event, m_callback, key,
                                         INFINITE, WT_EXECUTEDEFAULT)) {
            qDebug() << "RegisterWaitForSingleObject failed with error" << GetLastError();
            return;
        }

        //waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
        waitHandles.insert(key, HandlePair(event, waitHandle));
#endif
    }
}

#ifdef Q_OS_WINCE
void RegistryLayer::eventSignaled(void *handle)
{
    QMutexLocker locker(&localLock);

    HKEY key = waitHandles.key(QPair<::HANDLE, ::HANDLE>(handle, INVALID_HANDLE_VALUE), 0);

    if (key != 0)
        emitHandleChanged(key);
}
#endif

void RegistryLayer::openRegistryKey(RegistryHandle *handle)
{
    QMutexLocker locker(&localLock);

    if (!handle)
        return;

    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return;

    const QString fullPath = qConvertPath(m_basePath + handle->path);

    // Attempt to open registry key path
    HKEY key;
    long result = RegOpenKeyEx(HKEY_CURRENT_USER,
                                reinterpret_cast<const wchar_t*>(fullPath.utf16()),
                                0, KEY_ALL_ACCESS, &key);

    if (result == ERROR_SUCCESS) {
        hKeys.insert(handle, key);
    } else if (result == ERROR_FILE_NOT_FOUND) {
        // Key for handle does not exist in Registry, check if it is a value handle.
        int index = handle->path.lastIndexOf(QLatin1Char('/'), -1);

        const QString parentPath = handle->path.left(index);
        const QString valueName = handle->path.mid(index + 1);

        RegistryHandle *parentHandle = registryHandle(item(InvalidHandle, parentPath));
        if (!parentHandle)
            return;

        openRegistryKey(parentHandle);
        if (!hKeys.contains(parentHandle)) {
            removeHandle(Handle(parentHandle));
            return;
        }

        // Check if value exists.
        if (!children(Handle(parentHandle)).contains(valueName)) {
            removeHandle(Handle(parentHandle));
            return;
        }

        handle->valueHandle = true;

        hKeys.insert(handle, hKeys.value(parentHandle));

        removeHandle(Handle(parentHandle));
    }
}

bool RegistryLayer::createRegistryKey(RegistryHandle *handle)
{
    QMutexLocker locker(&localLock);

    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return false;

    const QString fullPath = qConvertPath(m_basePath + handle->path);

    // Attempt to open registry key path
    HKEY key;
    DWORD disposition;
    long result = RegCreateKeyEx(HKEY_CURRENT_USER,
                                reinterpret_cast<const wchar_t*>(fullPath.utf16()),
                                 0, 0,
                                 (m_volatileKeys ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE),
                                 KEY_ALL_ACCESS, 0, &key, &disposition);

    if (result == ERROR_SUCCESS)
        hKeys.insert(handle, key);

    return disposition == REG_CREATED_NEW_KEY;
}

bool RegistryLayer::removeSubTree(QValueSpacePublisher *creator, Handle handle)
{
    QMutexLocker locker(&localLock);

    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return false;

    QList<QString> paths = creators.value(creator);

    while (!paths.isEmpty()) {
        QString item = paths.takeFirst();
        if (!item.startsWith(rh->path))
            continue;

        removeRegistryValue(0, item);
        creators[creator].removeOne(item);

        int index = item.lastIndexOf(QLatin1Char('/'));
        if (index == -1)
            continue;

        item.truncate(index);
        if (!paths.contains(item))
            paths.append(item);
    }

    pruneEmptyKeys(rh);

    return true;
}

void RegistryLayer::pruneEmptyKeys(RegistryHandle *handle)
{
    QMutexLocker locker(&localLock);

    if (!children(Handle(handle)).isEmpty())
        return;

    QString path = handle->path;

    while (path != QLatin1String("/")) {
        int index = path.lastIndexOf(QLatin1Char('/'), -1);

        QString value = path.mid(index + 1);

        path.truncate(index);
        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        RegistryHandle *rh = registryHandle(item(InvalidHandle, path));

        openRegistryKey(rh);
        if (!hKeys.contains(rh)) {
            removeHandle(Handle(rh));
            return;
        }

        HKEY key = hKeys.value(rh);

        long result = RegDeleteKey(key, reinterpret_cast<const wchar_t*>(value.utf16()));
        if (result == ERROR_SUCCESS) {
            QList<QString> paths = handles.keys();
            while (!paths.isEmpty()) {
                const QString p = paths.takeFirst();

                if (p.startsWith(path))
                    closeRegistryKey(handles.value(p));
            }
        } else if (result != ERROR_FILE_NOT_FOUND) {
            return;
        }

        bool hasChildren = !children(Handle(rh)).isEmpty();

        removeHandle(Handle(rh));

        if (hasChildren)
            break;
    }
}

bool RegistryLayer::removeValue(QValueSpacePublisher *creator,
                                Handle handle,
                                const QString &subPath)
{
    QMutexLocker locker(&localLock);

    QString fullPath;

    if (handle == InvalidHandle) {
        fullPath = subPath;
    } else {
        RegistryHandle *rh = registryHandle(handle);
        if (!rh)
            return false;

        if (subPath == QLatin1String("/"))
            fullPath = rh->path;
        else if (rh->path.endsWith(QLatin1Char('/')) && subPath.startsWith(QLatin1Char('/')))
            fullPath = rh->path + subPath.mid(1);
        else if (!rh->path.endsWith(QLatin1Char('/')) && !subPath.startsWith(QLatin1Char('/')))
            fullPath = rh->path + QLatin1Char('/') + subPath;
        else
            fullPath = rh->path + subPath;
    }

    // permanent layer always removes items even if our records show that creator does not own it.
    if (!creators[creator].contains(fullPath) && (layerOptions() & QValueSpace::TransientLayer))
        return false;

    removeRegistryValue(0, fullPath);
    creators[creator].removeOne(fullPath);

    return true;
}

void RegistryLayer::addWatch(QValueSpacePublisher *, Handle)
{
}

void RegistryLayer::removeWatches(QValueSpacePublisher *, Handle)
{
}

bool RegistryLayer::supportsInterestNotification() const
{
    return false;
}

bool RegistryLayer::notifyInterest(Handle, bool)
{
    return false;
}
#include <registrylayer_win.moc>
QTM_END_NAMESPACE



