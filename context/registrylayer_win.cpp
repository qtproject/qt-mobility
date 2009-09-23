/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
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
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvaluespace.h"
#include "qvaluespaceobject.h"

#include <QSet>
#include <QStringList>
#include <QEventLoop>
#include <QTimer>

#include <QDebug>

// Define win32 version to pull in RegisterWaitForSingleObject and UnregisterWait.
#define _WIN32_WINNT 0x0500
#include <windows.h>

QT_BEGIN_NAMESPACE

class RegistryLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    RegistryLayer(const QByteArray &basePath, bool volatileKeys, WAITORTIMERCALLBACK callback);
    ~RegistryLayer();

    /* Common functions */
    bool startup(Type t);

    Handle item(Handle parent, const QByteArray &path);
    void removeHandle(Handle handle);
    void setProperty(Handle handle, Properties);

    bool value(Handle handle, QVariant *data);
    bool value(Handle handle, const QByteArray &subPath, QVariant *data);
    QSet<QByteArray> children(Handle handle);

    /* QValueSpaceItem functions */
    bool supportsRequests() const { return false; }
    bool notifyInterest(Handle handle, bool interested);
    bool syncRequests();

    /* QValueSpaceObject functions */
    bool setValue(QValueSpaceObject *creator, Handle handle, const QVariant &data);
    bool setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &path, const QVariant &data);
    bool removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath);
    bool removeSubTree(QValueSpaceObject *creator, Handle parent);
    void addWatch(QValueSpaceObject *creator, Handle handle);
    void removeWatches(QValueSpaceObject *creator, Handle parent);
    void sync();

public slots:
    void emitHandleChanged(void *hkey);

private:
    struct RegistryHandle {
        RegistryHandle(const QByteArray &p)
        :   path(p), valueHandle(false), refCount(1)
        {
        }

        QByteArray path;
        bool valueHandle;
        unsigned int refCount;
    };

    void openRegistryKey(RegistryHandle *handle);
    bool createRegistryKey(RegistryHandle *handle);
    bool removeRegistryValue(RegistryHandle *handle, const QByteArray &path);
    void closeRegistryKey(RegistryHandle *handle);
    void pruneEmptyKeys(RegistryHandle *handle);

    QByteArray m_basePath;
    bool m_volatileKeys;
    WAITORTIMERCALLBACK m_callback;

    QHash<QByteArray, RegistryHandle *> handles;

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
    QMap<HKEY, QPair<::HANDLE, ::HANDLE> > waitHandles;

    QMap<QValueSpaceObject *, QList<QByteArray> > creators;
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

    LayerOptions layerOptions() const;

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

    LayerOptions layerOptions() const;

    static NonVolatileRegistryLayer *instance();
};

Q_GLOBAL_STATIC(NonVolatileRegistryLayer, nonVolatileRegistryLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(NonVolatileRegistryLayer);

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

static QString qConvertPath(const QByteArray &path)
{
    QString fixedPath = QString::fromUtf8(path.constData(), path.length());

    while (fixedPath.endsWith(QChar('/')))
        fixedPath.chop(1);

    fixedPath.replace(QChar('/'), QChar('\\'));

    return fixedPath;
}

VolatileRegistryLayer::VolatileRegistryLayer()
:   RegistryLayer(QByteArray("Software/Nokia/QtMobility/volatileContext"), true,
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
    return QUuid(0x8ceb5811, 0x4968, 0x470f, 0x8f, 0xc2,
                 0x26, 0x47, 0x67, 0xe0, 0xbb, 0xd9);
}

unsigned int VolatileRegistryLayer::order()
{
    return 0x1000;
}

QAbstractValueSpaceLayer::LayerOptions VolatileRegistryLayer::layerOptions() const
{
    return NonPermanentLayer | WriteableLayer;
}

VolatileRegistryLayer *VolatileRegistryLayer::instance()
{
    return volatileRegistryLayer();
}

NonVolatileRegistryLayer::NonVolatileRegistryLayer()
:   RegistryLayer(QByteArray("Software/Nokia/QtMobility/nonVolatileContext"), false,
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
    return QUuid(0x8e29561c, 0xa0f0, 0x4e89, 0xba, 0x56,
                 0x08, 0x06, 0x64, 0xab, 0xc0, 0x17);
}

unsigned int NonVolatileRegistryLayer::order()
{
    return 0;
}

QAbstractValueSpaceLayer::LayerOptions NonVolatileRegistryLayer::layerOptions() const
{
    return PermanentLayer | WriteableLayer;
}

NonVolatileRegistryLayer *NonVolatileRegistryLayer::instance()
{
    return nonVolatileRegistryLayer();
}

RegistryLayer::RegistryLayer(const QByteArray &basePath, bool volatileKeys,
                             WAITORTIMERCALLBACK callback)
:   m_basePath(basePath), m_volatileKeys(volatileKeys), m_callback(callback)
{
    // Ensure that the m_basePath key exists and is non-volatile.
    HKEY key;
    RegCreateKeyEx(HKEY_CURRENT_USER, qConvertPath(m_basePath).utf16(),
                   0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, 0);

    RegCloseKey(key);
}

RegistryLayer::~RegistryLayer()
{
    QMutableHashIterator<QByteArray, RegistryHandle *> i(handles);
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
    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return false;

    return value(InvalidHandle, rh->path, data);
}

bool RegistryLayer::value(Handle handle, const QByteArray &subPath, QVariant *data)
{
    if (handle != InvalidHandle && !registryHandle(handle))
        return false;

    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);
    if (handle != InvalidHandle)
        while (path.startsWith('/'))
            path = path.mid(1);

    int index = path.lastIndexOf('/', -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = QString::fromUtf8(path.constData(), path.length());
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
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
    long result = RegQueryValueEx(key, value.utf16(), 0, 0, 0, &regSize);
    if (result == ERROR_FILE_NOT_FOUND) {
        *data = QVariant();
        if (createdHandle)
            removeHandle(handle);
        return false;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegQueryValueEx failed with error" << result;
        if (createdHandle)
            removeHandle(handle);
        return false;
    }

    BYTE *regData = new BYTE[regSize];
    DWORD regType;

    result = RegQueryValueEx(key, value.utf16(), 0, &regType, regData, &regSize);
    if (result != ERROR_SUCCESS) {
        qDebug() << "real RegQueryValueEx failed with error" << result;
        if (createdHandle)
            removeHandle(handle);
        return false;
    }

    switch (regType) {
    case REG_DWORD:
        *data = qVariantFromValue(*reinterpret_cast<int *>(regData));
        break;
    case REG_QWORD:
        *data = qVariantFromValue(*reinterpret_cast<qint64 *>(regData));
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
QSet<QByteArray> RegistryLayer::children(Handle handle)
{
    QSet<QByteArray> foundChildren;

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
        if (result == ERROR_NO_MORE_ITEMS)
            break;

        foundChildren << QString::fromWCharArray(subKey, subKeySize).toUtf8();
        ++i;
    } while (result == ERROR_SUCCESS);

    i = 0;
    do {
        TCHAR valueName[MAX_NAME_LENGTH];
        DWORD valueNameSize = MAX_NAME_LENGTH;

        result = RegEnumValue(key, i, valueName, &valueNameSize, 0, 0, 0, 0);
        if (result == ERROR_NO_MORE_ITEMS)
            break;

        foundChildren << QString::fromWCharArray(valueName, valueNameSize).toUtf8();
        ++i;
    } while (result == ERROR_SUCCESS);

    return foundChildren;
}

QAbstractValueSpaceLayer::Handle RegistryLayer::item(Handle parent, const QByteArray &path)
{
    QByteArray fullPath;

    // Fail on invalid path.
    if (path.isEmpty() || path.contains("//"))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = path;
    } else {
        RegistryHandle *rh = registryHandle(parent);
        if (!rh)
            return InvalidHandle;

        if (path == "/") {
            fullPath = rh->path;
        } else if (rh->path.endsWith('/') && path.startsWith('/'))
            fullPath = rh->path + path.mid(1);
        else if (!rh->path.endsWith('/') && !path.startsWith('/'))
            fullPath = rh->path + '/' + path;
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
    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return;

    if (properties & QAbstractValueSpaceLayer::Publish) {
        // Enable change notification for handle
        openRegistryKey(rh);
        if (!hKeys.contains(rh)) {
            // The key does not exist. Temporarily use the parent key as a proxy.
            QByteArray path = rh->path;
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

                int index = path.lastIndexOf('/');
                if (index == 0)
                    path.truncate(1);
                else
                    path.truncate(index);
            }
        }

        HKEY key = hKeys.value(rh);

        if (waitHandles.contains(key))
            return;

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

        waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
    }
    if (!(properties & QAbstractValueSpaceLayer::Publish)) {
        // Disable change notification for handle
        if (!hKeys.contains(rh))
            return;

        HKEY key = hKeys.value(rh);

        QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
        ::HANDLE event = wait.first;
        ::HANDLE waitHandle = wait.second;

        UnregisterWait(waitHandle);
        CloseHandle(event);
    }
}

void RegistryLayer::removeHandle(Handle handle)
{
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
    if (!hKeys.contains(handle))
        return;

    HKEY key = hKeys.take(handle);

    // Check if other handles are using this registry key.
    if (!hKeys.keys(key).isEmpty())
        return;

    QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);

    UnregisterWait(wait.second);
    CloseHandle(wait.first);

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

bool RegistryLayer::removeRegistryValue(RegistryHandle *handle, const QByteArray &subPath)
{
    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);

    int index = path.lastIndexOf('/', -1);

    bool createdHandle = false;

    RegistryHandle *rh;
    QString value;
    if (index == -1) {
        rh = handle;
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
        path.truncate(index);
        if (path.isEmpty())
            path.append('/');

        rh = registryHandle(item(handle ? Handle(handle) : InvalidHandle, path));

        createdHandle = true;
    }

    openRegistryKey(rh);
    if (!hKeys.contains(rh)) {
        if (createdHandle)
            removeHandle(Handle(rh));

        return false;
    }

    HKEY key = hKeys.value(rh);

    long result = RegDeleteValue(key, value.utf16());
    if (result == ERROR_FILE_NOT_FOUND) {
        result = qRegDeleteTree(key, value.utf16());
        if (result == ERROR_SUCCESS) {
            const QByteArray rootPath = rh->path;

            QList<QByteArray> paths = handles.keys();
            while (!paths.isEmpty()) {
                QByteArray p = paths.takeFirst();

                if (p.startsWith(rootPath))
                    closeRegistryKey(handles.value(p));
            }
        }
        if (result != ERROR_SUCCESS)
            qDebug() << "RegDeleteTree failed with error" << result;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegDeleteValue failed with error" << result;
    }

    if (createdHandle)
        removeHandle(Handle(rh));

    return result == ERROR_SUCCESS;
}

bool RegistryLayer::setValue(QValueSpaceObject *creator, Handle handle, const QVariant &data)
{
    return setValue(creator, handle, QByteArray(), data);
}

bool RegistryLayer::setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath, const QVariant &data)
{
    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return false;

    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);

    int index = path.lastIndexOf('/', -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
        path.truncate(index);

        if (path.isEmpty())
            path.append('/');

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

    DWORD regType;
    const BYTE *regData;
    DWORD regSize;
    const void *toDelete;

    switch (data.type()) {
    case QVariant::Int: {
        regType = REG_DWORD;
        DWORD *temp = new DWORD;
        *temp = data.toInt();
        toDelete = regData = reinterpret_cast<const BYTE *>(temp);
        regSize = sizeof(DWORD);
        break;
    }
    case QVariant::LongLong: {
        regType = REG_QWORD;
        qint64 *temp = new qint64;
        *temp = data.toLongLong();
        toDelete = regData = reinterpret_cast<const BYTE *>(temp);
        regSize = sizeof(qint64);
        break;
    }
    case QVariant::String: {
        regType = REG_SZ;
        int length = data.toString().length() + 1;
        wchar_t *temp = new wchar_t[length];
        data.toString().toWCharArray(temp);
        temp[length - 1] = L'\0';
        toDelete = regData = reinterpret_cast<const BYTE *>(temp);
        regSize = length * sizeof(wchar_t);
        break;
    }
    case QVariant::StringList: {
        regType = REG_MULTI_SZ;
        QString joined = data.toStringList().join(QChar('\0'));
        int length = joined.length() + 2;
        wchar_t *temp = new wchar_t[length];
        joined.toWCharArray(temp);
        temp[length - 2] = L'\0';
        temp[length - 1] = L'\0';
        toDelete = regData = reinterpret_cast<const BYTE *>(temp);
        regSize = length * sizeof(wchar_t);
        break;
    }
    case QVariant::ByteArray: {
        regType = REG_BINARY;
        QByteArray *temp = new QByteArray(data.toByteArray());
        regData = reinterpret_cast<const BYTE *>(temp->constData());
        toDelete = temp;
        regSize = temp->length();
        break;
    }
    default: {
        regType = REG_NONE;
        QByteArray *temp = new QByteArray;
        QDataStream stream(temp, QIODevice::WriteOnly | QIODevice::Truncate);
        stream << data;
        regData = reinterpret_cast<const BYTE *>(temp->constData());
        toDelete = temp;
        regSize = temp->length();
        break;
    }
    };

    long result = RegSetValueEx(key, value.utf16(), 0, regType, regData, regSize);

    QByteArray fullPath(rh->path);
    if (fullPath != "/")
        fullPath.append('/');

    fullPath.append(value.toUtf8());

    if (!creators[creator].contains(fullPath))
        creators[creator].append(fullPath);

    delete[] toDelete;

    if (createdHandle)
        removeHandle(Handle(rh));

    return result == ERROR_SUCCESS;
}

void RegistryLayer::sync()
{
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
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
    }
}

void RegistryLayer::emitHandleChanged(void *k)
{
    HKEY key = reinterpret_cast<HKEY>(k);

    QList<RegistryHandle *> changedHandles = hKeys.keys(key);
    if (changedHandles.isEmpty()) {
        QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
        UnregisterWait(wait.second);
        CloseHandle(wait.first);
        return;
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

    QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);

    ::HANDLE event = wait.first;
    ::HANDLE waitHandle = wait.second;

    UnregisterWait(waitHandle);

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

    if (!RegisterWaitForSingleObject(&waitHandle, event, m_callback, key,
                                     INFINITE, WT_EXECUTEDEFAULT)) {
      qDebug() << "RegisterWaitForSingleObject failed with error" << GetLastError();
      return;
    }

    waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
}

void RegistryLayer::openRegistryKey(RegistryHandle *handle)
{
    if (!handle)
        return;

    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return;

    const QString fullPath = qConvertPath(m_basePath + handle->path);

    // Attempt to open registry key path
    HKEY key;
    long result = RegOpenKeyEx(HKEY_CURRENT_USER, fullPath.utf16(),
                               0, KEY_ALL_ACCESS, &key);

    if (result == ERROR_SUCCESS) {
        hKeys.insert(handle, key);
    } else if (result == ERROR_FILE_NOT_FOUND) {
        // Key for handle does not exist in Registry, check if it is a value handle.
        int index = handle->path.lastIndexOf('/', -1);

        QByteArray parentPath = handle->path.left(index);
        QByteArray valueName = handle->path.mid(index + 1);

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
    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return false;

    const QString fullPath = qConvertPath(m_basePath + handle->path);

    // Attempt to open registry key path
    HKEY key;
    DWORD disposition;
    long result = RegCreateKeyEx(HKEY_CURRENT_USER, fullPath.utf16(),
                                 0, 0,
                                 (m_volatileKeys ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE),
                                 KEY_ALL_ACCESS, 0, &key, &disposition);

    if (result == ERROR_SUCCESS)
        hKeys.insert(handle, key);

    return disposition == REG_CREATED_NEW_KEY;
}

bool RegistryLayer::removeSubTree(QValueSpaceObject *creator, Handle handle)
{
    RegistryHandle *rh = registryHandle(handle);
    if (!rh)
        return false;

    QList<QByteArray> paths = creators.value(creator);

    while (!paths.isEmpty()) {
        QByteArray item = paths.takeFirst();
        if (!item.startsWith(rh->path))
            continue;

        removeRegistryValue(0, item);
        creators[creator].removeOne(item);

        int index = item.lastIndexOf('/');
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
    if (!children(Handle(handle)).isEmpty())
        return;

    QByteArray path = handle->path;

    while (path != "/") {
        int index = path.lastIndexOf('/', -1);

        QString value =
            QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));

        path.truncate(index);
        if (path.isEmpty())
            path.append('/');

        RegistryHandle *rh = registryHandle(item(InvalidHandle, path));

        openRegistryKey(rh);
        if (!hKeys.contains(rh)) {
            removeHandle(Handle(rh));
            return;
        }

        HKEY key = hKeys.value(rh);

        long result = RegDeleteKey(key, value.utf16());
        if (result == ERROR_SUCCESS) {
            QList<QByteArray> paths = handles.keys();
            while (!paths.isEmpty()) {
                QByteArray p = paths.takeFirst();

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

bool RegistryLayer::removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath)
{
    QByteArray fullPath;

    if (handle == InvalidHandle) {
        fullPath = subPath;
    } else {
        RegistryHandle *rh = registryHandle(handle);
        if (!rh)
            return false;

        if (subPath == "/")
            fullPath = rh->path;
        else if (rh->path.endsWith('/') && subPath.startsWith('/'))
            fullPath = rh->path + subPath.mid(1);
        else if (!rh->path.endsWith('/') && !subPath.startsWith('/'))
            fullPath = rh->path + '/' + subPath;
        else
            fullPath = rh->path + subPath;
    }

    // permanent layer always removes items even if our records show that creator does not own it.
    if (!creators[creator].contains(fullPath) && (layerOptions() & NonPermanentLayer))
        return false;

    removeRegistryValue(0, fullPath);
    creators[creator].removeOne(fullPath);

    return true;
}

void RegistryLayer::addWatch(QValueSpaceObject *, Handle)
{
}

void RegistryLayer::removeWatches(QValueSpaceObject *, Handle)
{
}

bool RegistryLayer::requestSetValue(Handle, const QVariant &)
{
    return false;
}

bool RegistryLayer::requestSetValue(Handle, const QByteArray &, const QVariant &)
{
    return false;
}

bool RegistryLayer::requestRemoveValue(Handle, const QByteArray &)
{
    return false;
}

bool RegistryLayer::notifyInterest(Handle, bool)
{
    return false;
}

bool RegistryLayer::syncRequests()
{
    return true;
}

QT_END_NAMESPACE

#include <registrylayer_win.moc>

