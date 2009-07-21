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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvaluespace.h"

#include <QSet>
#include <QStringList>
#include <QEventLoop>
#include <QTimer>

#include <QDebug>

#include <windows.h>

class RegistryLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    RegistryLayer();
    ~RegistryLayer();

    virtual QString name();
    virtual bool startup(Type t);
    virtual bool restart();
    virtual void shutdown();
    virtual QUuid id();
    virtual unsigned int order();
    virtual bool value(HANDLE handle, QVariant *data);
    virtual bool value(HANDLE handle, const QByteArray &subPath, QVariant *data);
    virtual QSet<QByteArray> children(HANDLE handle);
    virtual HANDLE item(HANDLE parent, const QByteArray &path);
    virtual void setProperty(HANDLE handle, Properties);
    virtual void remHandle(HANDLE);
    virtual bool remove(HANDLE);
    virtual bool remove(HANDLE, const QByteArray &);
    virtual bool setValue(HANDLE handle, const QVariant &data);
    virtual bool setValue(HANDLE handle, const QByteArray &subPath, const QVariant &data);
    virtual bool syncChanges();

    void emitHandleChanged(HKEY key);

    static RegistryLayer *instance();

private:
    void openRegistryKey(HANDLE handle);
    void createRegistryKey(HANDLE handle);

    QHash<QByteArray, HANDLE> handles;
    QList<HANDLE> valueHandles;
    QMap<HANDLE, int> refCount;

    QMap<HANDLE, HKEY> hKeys;
    QMap<HKEY, QPair<::HANDLE, ::HANDLE> > waitHandles;
};

#include <registrylayer_win.moc>

Q_GLOBAL_STATIC(RegistryLayer, registryLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(RegistryLayer);

void CALLBACK qRegistryLayerCallback(PVOID lpParameter, BOOLEAN timedOut)
{
    registryLayer()->emitHandleChanged(reinterpret_cast<HKEY>(lpParameter));
}

static QString qConvertPath(const QByteArray &path)
{
    QString fixedPath = QString::fromUtf8(path.constData(), path.length());

    while (fixedPath.endsWith(QChar('/')))
        fixedPath.chop(1);

    fixedPath.replace(QChar('/'), QChar('\\'));

    return fixedPath;
}

RegistryLayer::RegistryLayer()
{
}

RegistryLayer::~RegistryLayer()
{
    while (!valueHandles.isEmpty())
        remHandle(valueHandles.takeFirst());

    while (!handles.isEmpty())
        remHandle(*handles.begin());
}

QString RegistryLayer::name()
{
    return QLatin1String("Registry Layer");
}

bool RegistryLayer::startup(Type t)
{
    // connect to registry?

    return true;
}

bool RegistryLayer::restart()
{
    // reconnect to registry?

    return true;
}

void RegistryLayer::shutdown()
{
}

QUuid RegistryLayer::id()
{
    return QUuid();
}

unsigned int RegistryLayer::order()
{
    return 0;
}

bool RegistryLayer::value(HANDLE handle, QVariant *data)
{
    return value(InvalidHandle, handles.key(handle), data);
}

bool RegistryLayer::value(HANDLE handle, const QByteArray &subPath, QVariant *data)
{
    if (handle != InvalidHandle && !handles.values().contains(handle))
        return false;

    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);

    int index = path.lastIndexOf('/', -1);

    QString value;
    if (index == -1) {
        value = QString::fromUtf8(path.constData(), path.length());
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
        path.truncate(index);

        handle = item(handle, path);
    }

    openRegistryKey(handle);
    if (!hKeys.contains(handle))
        return false;

    HKEY key = hKeys.value(handle);

    DWORD regSize = 0;
    long result = RegQueryValueEx(key, value.utf16(), 0, 0, 0, &regSize);
    if (result == ERROR_FILE_NOT_FOUND) {
        *data = QVariant();
        return false;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegQueryValueEx failed with error" << result;
        return false;
    }

    BYTE *regData = new BYTE[regSize];
    DWORD regType;

    result = RegQueryValueEx(key, value.utf16(), 0, &regType, regData, &regSize);
    if (result != ERROR_SUCCESS) {
        qDebug() << "real RegQueryValueEx failed with error" << result;
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
        return false;
        break;
    }

    delete[] regData;
    return true;
}

#define MAX_KEY_LENGTH 255
#define MAX_NAME_LENGTH 16383
QSet<QByteArray> RegistryLayer::children(HANDLE handle)
{
    QSet<QByteArray> foundChildren;

    openRegistryKey(handle);
    if (!hKeys.contains(handle))
        return foundChildren;

    HKEY key = hKeys.value(handle);
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

QAbstractValueSpaceLayer::HANDLE RegistryLayer::item(HANDLE parent, const QByteArray &path)
{
    QByteArray fullPath;

    // Fail on invalid path.
    if (path.contains("//"))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = path;
    } else {
        QByteArray parentPath = handles.key(parent);

        if (parentPath.endsWith('/') && path.startsWith('/'))
            fullPath = parentPath + path.mid(1);
        else if (!parentPath.endsWith('/') && !path.startsWith('/'))
            fullPath = parentPath + '/' + path;
        else
            fullPath = parentPath + path;
    }

    if (handles.contains(fullPath)) {
        HANDLE handle = handles.value(fullPath);
        ++refCount[handle];
        return handle;
    }

    // Create random handle for path
    HANDLE handle = qrand();

    QList<HANDLE> allocatedHandles = handles.values();
    while (allocatedHandles.contains(handle) && handle != InvalidHandle)
        handle = qrand();

    if (handle == InvalidHandle)
        return InvalidHandle;

    handles.insert(fullPath, handle);
    refCount.insert(handle, 1);

    return handle;
}

void RegistryLayer::setProperty(HANDLE handle, Properties properties)
{
    if (!handles.values().contains(handle))
        return;

    if (properties & QAbstractValueSpaceLayer::Publish) {
        // Enable change notification for handle
        openRegistryKey(handle);
        if (!hKeys.contains(handle))
            return;

        HKEY key = hKeys.value(handle);

        if (waitHandles.contains(key))
            return;

        ::HANDLE event = CreateEvent(0, false, false, 0);
        if (event == 0) {
            qDebug() << "CreateEvent failed with error" << GetLastError();
            return;
        }

        DWORD filter = REG_NOTIFY_CHANGE_LAST_SET;
        // REG_NOTIFY_CHANGE_NAME REG_NOTIFY_CHANGE_ATTRIBUTES REG_NOTIFY_CHANGE_LAST_SET
        long result = RegNotifyChangeKeyValue(key, true, filter, event, true);

        if (result != ERROR_SUCCESS) {
            qDebug() << "RegNotifyChangeKeyValue failed with error" << result;
            return;
        }

        ::HANDLE waitHandle;
        bool ret = RegisterWaitForSingleObject(&waitHandle, event, &qRegistryLayerCallback, key, INFINITE, WT_EXECUTEDEFAULT);
        if (!ret) {
            qDebug() << "RegisterWaitForSingleObject failed with error" << GetLastError();
            return;
        }

        waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
    }
    if (!(properties & QAbstractValueSpaceLayer::Publish)) {
        // Disable change notification for handle
        if (!hKeys.contains(handle))
            return;

        HKEY key = hKeys.value(handle);

        QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);
        ::HANDLE event = wait.first;
        ::HANDLE waitHandle = wait.second;

        UnregisterWait(waitHandle);
        CloseHandle(event);
    }
}

void RegistryLayer::remHandle(HANDLE handle)
{
    if (!handles.values().contains(handle))
        return;

    if (--refCount[handle])
        return;

    refCount.remove(handle);
    handles.remove(handles.key(handle));
    valueHandles.removeOne(handle);

    HKEY key = hKeys.take(handle);

    QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);

    UnregisterWait(wait.second);
    CloseHandle(wait.first);

    RegCloseKey(key);
}

bool RegistryLayer::remove(HANDLE handle)
{
    return remove(InvalidHandle, handles.key(handle));
}

static LSTATUS qRegDeleteTree(HKEY hKey, LPCTSTR lpSubKey)
{
    HKEY key;
    long result = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &key);
    if (result != ERROR_SUCCESS)
        return result;

    do {
        TCHAR subKey[MAX_KEY_LENGTH];
        DWORD subKeySize = MAX_KEY_LENGTH;

        long result = RegEnumKeyEx(key, 0, subKey, &subKeySize, 0, 0, 0, 0);
        if (result == ERROR_NO_MORE_ITEMS)
            break;

        result = qRegDeleteTree(key, subKey);
    } while (result == ERROR_SUCCESS);

    RegCloseKey(key);

    if (result != ERROR_NO_MORE_ITEMS && result != ERROR_SUCCESS)
        return result;

    return RegDeleteKey(hKey, lpSubKey);
}

bool RegistryLayer::remove(HANDLE handle, const QByteArray &subPath)
{
    if (handle != InvalidHandle && !handles.values().contains(handle))
        return false;

    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);

    int index = path.lastIndexOf('/', -1);

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
        path.truncate(index);

        handle = item(handle, path);
    }

    openRegistryKey(handle);
    if (!hKeys.contains(handle))
        return false;

    HKEY key = hKeys.value(handle);

    long result = RegDeleteValue(key, value.utf16());
    if (result == ERROR_FILE_NOT_FOUND) {
        result = qRegDeleteTree(key, value.utf16());
        if (result != ERROR_SUCCESS)
            qDebug() << "RegDeleteTree failed with error" << result;
    } else if (result != ERROR_SUCCESS) {
        qDebug() << "RegDeleteValue failed with error" << result;
    }

    return result == ERROR_SUCCESS;
}

bool RegistryLayer::setValue(HANDLE handle, const QVariant &data)
{
    return setValue(handle, QByteArray(), data);
}

bool RegistryLayer::setValue(HANDLE handle, const QByteArray &subPath, const QVariant &data)
{
    if (!handles.values().contains(handle))
        return false;

    QByteArray path(subPath);
    while (path.endsWith('/'))
        path.chop(1);

    int index = path.lastIndexOf('/', -1);

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = QString::fromUtf8(path.constData() + (index + 1), path.length() - (index + 1));
        path.truncate(index);

        handle = item(handle, path);
    }

    createRegistryKey(handle);
    if (!hKeys.contains(handle))
        return false;

    HKEY key = hKeys.value(handle);

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

    delete[] toDelete;

    return result == ERROR_SUCCESS;
}

bool RegistryLayer::syncChanges()
{
    bool failed = false;

    // Wait for change notification callbacks before returning
    QEventLoop loop;
    connect(this, SIGNAL(handleChanged(uint)), &loop, SLOT(quit()));
    bool wait = false;

    QList<HKEY> keys = hKeys.values();
    while (!keys.isEmpty()) {
        HKEY key = keys.takeFirst();

        if (!wait && waitHandles.contains(key))
            wait = true;

        failed |= (RegFlushKey(key) != ERROR_SUCCESS);
    }

    if (wait) {
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
    }

    return failed;
}

void RegistryLayer::emitHandleChanged(HKEY key)
{
    QList<HANDLE> changedHandles = hKeys.keys(key);
    if (changedHandles.isEmpty())
        return;

    while (!changedHandles.isEmpty())
        emit handleChanged(changedHandles.takeFirst());

    QPair<::HANDLE, ::HANDLE> wait = waitHandles.take(key);

    ::HANDLE event = wait.first;
    ::HANDLE waitHandle = wait.second;

    UnregisterWait(waitHandle);

    long result = RegNotifyChangeKeyValue(key, true, REG_NOTIFY_CHANGE_NAME |
                                                     REG_NOTIFY_CHANGE_ATTRIBUTES |
                                                     REG_NOTIFY_CHANGE_LAST_SET,
                                          event, true);

    if (result != ERROR_SUCCESS) {
        qDebug() << "RegNotifyChangeKeyValue failed with error" << result;
        return;
    }

    bool ret = RegisterWaitForSingleObject(&waitHandle, event, &qRegistryLayerCallback, key, INFINITE, WT_EXECUTEDEFAULT);
    if (!ret) {
      qDebug() << "RegisterWaitForSingleObject failed with error" << GetLastError();
      return;
    }

    waitHandles.insert(key, QPair<::HANDLE, ::HANDLE>(event, waitHandle));
}

RegistryLayer *RegistryLayer::instance()
{
    return registryLayer();
}

void RegistryLayer::openRegistryKey(HANDLE handle)
{
    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return;

    // Check if handle is valid.
    if (!handles.values().contains(handle))
        return;

    QByteArray path = handles.key(handle);

    const QString fullPath = qConvertPath(QByteArray("Software/Nokia/QtMobility/context") + path);

    // Attempt to open registry key path
    HKEY key;
    long result = RegOpenKeyEx(HKEY_CURRENT_USER, fullPath.utf16(),
                               0, KEY_ALL_ACCESS, &key);

    if (result == ERROR_SUCCESS) {
        hKeys.insert(handle, key);
    } else if (result == ERROR_FILE_NOT_FOUND) {
        // Key for handle does not exist in Registry, check if it is a value handle.
        int index = path.lastIndexOf('/', -1);

        QByteArray parentPath = path.left(index);
        QByteArray valueName = path.mid(index + 1);

        HANDLE parentHandle = item(InvalidHandle, parentPath);
        openRegistryKey(parentHandle);
        if (!hKeys.contains(parentHandle))
            return;

        // Check if value exists.
        if (!children(parentHandle).contains(valueName))
            return;

        if (!valueHandles.contains(handle))
            valueHandles.append(handle);
        hKeys.insert(handle, hKeys.value(parentHandle));
    }
}

void RegistryLayer::createRegistryKey(HANDLE handle)
{
    // Check if HKEY for this handle already exists.
    if (hKeys.contains(handle))
        return;

    // Check if handle is valid.
    if (!handles.values().contains(handle))
        return;

    const QByteArray path = handles.key(handle);

    const QString fullPath = qConvertPath(QByteArray("Software/Nokia/QtMobility/context") + path);

    // Attempt to open registry key path
    HKEY key;
    long result = RegCreateKeyEx(HKEY_CURRENT_USER, fullPath.utf16(),
                                 0, 0, REG_OPTION_VOLATILE,
                                 KEY_ALL_ACCESS, 0, &key, 0);

    if (result == ERROR_SUCCESS)
        hKeys.insert(handle, key);
}

class QValueSpaceObjectPrivate
{
public:
    QAbstractValueSpaceLayer::HANDLE handle;
    QString objectPath;
};

QValueSpaceObject::QValueSpaceObject(const char *objectPath, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate)
{
    d->objectPath = objectPath;

    RegistryLayer *layer = registryLayer();

    d->handle = layer->item(QAbstractValueSpaceLayer::InvalidHandle, objectPath);
}

QValueSpaceObject::QValueSpaceObject(const QString &objectPath, QObject *parent)
:    QObject(parent), d(new QValueSpaceObjectPrivate)
{
    d->objectPath = objectPath;

    RegistryLayer *layer = registryLayer();

    d->handle = layer->item(QAbstractValueSpaceLayer::InvalidHandle, objectPath.toUtf8());
}

QValueSpaceObject::QValueSpaceObject(const QByteArray &objectPath, QObject *parent)
:    QObject(parent), d(new QValueSpaceObjectPrivate)
{
    d->objectPath = objectPath;

    RegistryLayer *layer = registryLayer();

    d->handle = layer->item(QAbstractValueSpaceLayer::InvalidHandle, objectPath);
}

QValueSpaceObject::~QValueSpaceObject()
{
    delete d;
}

QString QValueSpaceObject::objectPath() const
{
    return d->objectPath;
}

void QValueSpaceObject::sync()
{
    registryLayer()->syncChanges();
}

void QValueSpaceObject::setAttribute(const char *attribute, const QVariant &data)
{
    registryLayer()->setValue(d->handle, QByteArray(attribute), data);
}

void QValueSpaceObject::setAttribute(const QString &attribute, const QVariant &data)
{
    registryLayer()->setValue(d->handle, attribute.toUtf8(), data);
}

void QValueSpaceObject::setAttribute(const QByteArray &attribute, const QVariant &data)
{
    registryLayer()->setValue(d->handle, attribute, data);
}

void QValueSpaceObject::removeAttribute(const char *attribute)
{
    registryLayer()->remove(d->handle, QByteArray(attribute));
}

void QValueSpaceObject::removeAttribute(const QString &attribute)
{
    registryLayer()->remove(d->handle, attribute.toUtf8());
}

void QValueSpaceObject::removeAttribute(const QByteArray &attribute)
{
    registryLayer()->remove(d->handle, attribute);
}

void QValueSpaceObject::connectNotify(const char *method)
{
    QObject::connectNotify(method);
}
