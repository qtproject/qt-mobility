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

#include "contextkitlayer_p.h"

#include <QDebug>
#include <QCoreApplication>

QTM_BEGIN_NAMESPACE

using namespace QValueSpace;

ContextKitPath::ContextKitPath()
{
}

ContextKitPath::ContextKitPath(QString path)
{
    bool hasSlash = path.contains(QLatin1Char('/'));
    bool hasDot = path.contains(QLatin1Char('.'));
    bool startingSlash = path.startsWith(QLatin1Char('/'));

    if (startingSlash)
        path = path.mid(1);

    if (path == "" || path == "/") {
        slashPath = true;
        dotPath = false;
    } else if ((hasSlash && !hasDot) || (hasSlash && startingSlash)) {
        parts = path.split(QLatin1Char('/'));
        slashPath = true;
        dotPath = false;
    } else if (hasDot && !hasSlash) {
        parts = path.split(QLatin1Char('.'));
        dotPath = true;
        slashPath = false;
    } else {
        // don't know what it is?
        qWarning() << "ContextKit: path doesn't seem to obey any known scheme: "
                   << path;
    }
}

ContextKitPath::ContextKitPath(const ContextKitPath &other)
{
    parts = other.parts;
    dotPath = other.dotPath;
    slashPath = other.slashPath;
}

ContextKitPath &ContextKitPath::operator=(const ContextKitPath &other)
{
    parts = other.parts;
    dotPath = other.dotPath;
    slashPath = other.slashPath;
    return *this;
}

QString ContextKitPath::toCKPath() const
{
    ContextKitPath lastPath("/nil");
    foreach (const QString &k, ContextRegistryInfo::instance()->listKeys()) {
        ContextKitPath p(k);
        if (this->includes(p)) {
            lastPath = p;
            if (p.wasDotPath())
                break;
        }
    }

    if (lastPath.wasDotPath()) {
        return parts.join(".");
    } else {
        return "/" + parts.join("/");
    }
}

bool ContextKitPath::isRegistered() const
{
    foreach (const QString &k, ContextRegistryInfo::instance()->listKeys()) {
        ContextKitPath p(k);
        if (*this == p)
            return true;
    }
    return false;
}

ContextKitPath ContextKitPath::operator+(const QString &str) const
{
    ContextKitPath p(*this);
    ContextKitPath s(str);
    foreach (QString pt, s.parts)
        p.parts.append(pt);

    return p;
}

ContextKitPath ContextKitPath::operator-(const ContextKitPath &other) const
{
    if (other.parts.size() > parts.size()) return ContextKitPath();
    if (other.parts.size() == 0) return ContextKitPath(*this);
    if (parts.size() == 0) return ContextKitPath();

    ContextKitPath p(*this);
    ContextKitPath q(other);
    while (p.parts.size() > 0 && q.parts.size() > 0 &&
           p.parts.first() == q.parts.first()) {
        p.parts.removeFirst();
        q.parts.removeFirst();
    }
    return p;
}

QString ContextKitPath::toQtPath() const
{
    return "/" + parts.join("/");
}

bool ContextKitPath::operator==(const ContextKitPath &other) const
{
    if (parts.size() != other.parts.size())
        return false;

    for (int i = 0; i < parts.size(); i++)
        if (other.parts.at(i) != parts.at(i))
            return false;

    return true;
}

bool ContextKitPath::includes(ContextKitPath &other) const
{
    // can't include things smaller than you
    if (other.parts.size() < parts.size())
        return false;

    // root path includes all others
    if (parts.size() == 0)
        return true;

    for (int i = 0; i < parts.size(); i++)
        if (other.parts.at(i) != parts.at(i))
            return false;

    return true;
}

// borrowed from qt/corelib/io/qsettings_mac
static QString comify(const QString &organization)
{
    for (int i = organization.size() - 1; i >= 0; --i) {
        QChar ch = organization.at(i);
        if (ch == QLatin1Char('.') || ch == QChar(0x3002) || ch == QChar(0xff0e)
                || ch == QChar(0xff61)) {
            QString suffix = organization.mid(i + 1).toLower();
            if (suffix.size() == 2 || suffix == QLatin1String("com")
                    || suffix == QLatin1String("org") || suffix == QLatin1String("net")
                    || suffix == QLatin1String("edu") || suffix == QLatin1String("gov")
                    || suffix == QLatin1String("mil") || suffix == QLatin1String("biz")
                    || suffix == QLatin1String("info") || suffix == QLatin1String("name")
                    || suffix == QLatin1String("pro") || suffix == QLatin1String("aero")
                    || suffix == QLatin1String("coop") || suffix == QLatin1String("museum")) {
                QString result = organization;
                result.replace(QLatin1Char('/'), QLatin1Char(' '));
                return result;
            }
            break;
        }
        int uc = ch.unicode();
        if ((uc < 'a' || uc > 'z') && (uc < 'A' || uc > 'Z'))
            break;
    }

    QString domain;
    for (int i = 0; i < organization.size(); ++i) {
        QChar ch = organization.at(i);
        int uc = ch.unicode();
        if ((uc >= 'a' && uc <= 'z') || (uc >= '0' && uc <= '9')) {
            domain += ch;
        } else if (uc >= 'A' && uc <= 'Z') {
            domain += ch.toLower();
        } else {
           domain += QLatin1Char(' ');
        }
    }
    domain = domain.simplified();
    domain.replace(QLatin1Char(' '), QLatin1Char('-'));
    if (!domain.isEmpty())
        domain.append(QLatin1String(".com"));
    return domain;
}

void ContextKitHandle::insertRead(const ContextKitPath &path)
{
    if (!readProps.contains(path.toQtPath())) {
        ContextProperty *prop = new ContextProperty(path.toCKPath());
        connect(prop, SIGNAL(valueChanged()),
                this, SIGNAL(valueChanged()));
        readProps.insert(path.toQtPath(), prop);
    }
}

void ContextKitHandle::updateSubtrees()
{
    foreach (const QString &k, ContextRegistryInfo::instance()->listKeys()) {
        ContextKitPath p(k);
        if (path.includes(p))
            insertRead(p);
    }
}

ContextKitHandle::ContextKitHandle(ContextKitHandle *parent, const QString &path)
{
    this->path = ContextKitPath(path);

    updateSubtrees();
    connect(ContextRegistryInfo::instance(), SIGNAL(changed()),
            this, SLOT(updateSubtrees()));

    QString javaPackageName;
    int curPos = 0;
    int nextDot;

    QString domainName = comify(QCoreApplication::organizationDomain());
    if (domainName.isEmpty()) {
        domainName = QLatin1String("unknown-organization.nokia.com");
        qWarning("No organization name specified, registering on DBUS with "
                 "'com.nokia.unknown-organization'");
    }

    while ((nextDot = domainName.indexOf(QLatin1Char('.'), curPos)) != -1) {
        javaPackageName.prepend(domainName.mid(curPos, nextDot - curPos));
        javaPackageName.prepend(QLatin1Char('.'));
        curPos = nextDot + 1;
    }
    javaPackageName.prepend(domainName.mid(curPos));
    javaPackageName = javaPackageName.toLower();
    if (curPos == 0)
        javaPackageName.prepend(QLatin1String("com."));

    javaPackageName += QLatin1Char('.');
    QString app = QCoreApplication::applicationName();
    if (app.size() < 1 || app.at(0) == QLatin1Char(' ')) {
        javaPackageName += "unknown-application";
        qWarning("No application name specified, registering on DBUS as "
                 "'unknown-application'");
    } else {
        app.replace(QLatin1Char(' '), QLatin1Char('-'));
        javaPackageName += app;
    }

    service = new ContextProvider::Service(QDBusConnection::SessionBus,
                                           javaPackageName);
}

ContextKitHandle::~ContextKitHandle()
{
    foreach (ContextProperty *prop, readProps.values())
        delete prop;
    delete service;
}

bool ContextKitHandle::value(const QString &path, QVariant *data)
{
    ContextKitPath p = this->path + path;

    ContextProperty *prop = readProps.value(p.toQtPath());
    if (prop) {
        *data = prop->value();
        return true;
    } else {
        return false;
    }
}

bool ContextKitHandle::setValue(const QString &path, const QVariant &data)
{
    ContextKitPath p = this->path + path;

    ContextProvider::Property *prop = writeProps.value(p.toQtPath());
    if (!prop) {
        service->stop();

        QString pth = p.toCKPath();
        qDebug() << "creating new property with path " << pth;

        if (!p.isRegistered())
            qCritical("ContextKit: providing an unregistered path, clients "
                      "may not be able to connect to %s", qPrintable(pth));

        prop = new ContextProvider::Property(*service, pth);
        writeProps.insert(p.toQtPath(), prop);

        service->start();
    }

    prop->setValue(data);
    return true;
}

bool ContextKitHandle::unsetValue(const QString &path)
{
    ContextKitPath p = this->path + path;

    ContextProvider::Property *prop = writeProps.value(p.toQtPath());
    if (!prop) {
        service->stop();

        QString pth = p.toCKPath();
        qDebug() << "creating new property with path " << pth;

        if (!p.isRegistered())
            qCritical("ContextKit: providing an unregistered path, clients "
                      "may not be able to connect to %s", qPrintable(pth));

        prop = new ContextProvider::Property(*service, pth);
        writeProps.insert(p.toQtPath(), prop);

        service->start();
    }

    prop->unsetValue();
    return true;
}

void ContextKitHandle::subscribe()
{
    foreach (ContextProperty *p, readProps.values())
        p->subscribe();
}

void ContextKitHandle::unsubscribe()
{
    foreach (ContextProperty *p, readProps.values())
        p->unsubscribe();
}

QSet<QString> ContextKitHandle::children()
{
    QSet<QString> kids;

    foreach (const QString &qp, readProps.keys()) {
        ContextKitPath pth(qp);
        pth = pth - this->path;
        if (pth.size() > 0)
            kids.insert(pth.at(0));
    }
    return kids;
}

QVALUESPACE_AUTO_INSTALL_LAYER(ContextKitLayer);

ContextKitLayer::ContextKitLayer()
{
}

ContextKitLayer::~ContextKitLayer()
{
}

Q_GLOBAL_STATIC(ContextKitLayer, contextKitLayer);
ContextKitLayer *ContextKitLayer::instance()
{
    return contextKitLayer();
}

ContextKitHandle *ContextKitLayer::handleToCKHandle(Handle handle)
{
    ContextKitHandle *ckh = NULL;
    if (handle != InvalidHandle)
        ckh = reinterpret_cast<ContextKitHandle*>(handle);
    return ckh;
}

void ContextKitLayer::sync()
{
    QCoreApplication::processEvents();
}

QString ContextKitLayer::name()
{
    return "ContextKit Layer";
}

bool ContextKitLayer::startup(Type)
{
    return true;
}

QUuid ContextKitLayer::id()
{
    return QVALUESPACE_CONTEXTKIT_LAYER;
}

unsigned int ContextKitLayer::order()
{
    return 0;
}

LayerOptions ContextKitLayer::layerOptions() const
{
    return TransientLayer | ReadOnlyLayer;
}

QAbstractValueSpaceLayer::Handle ContextKitLayer::item(Handle parent,
                                                       const QString &subPath)
{
    ContextKitHandle *parentHandle = handleToCKHandle(parent);
    ContextKitHandle *h = new ContextKitHandle(parentHandle, subPath);
    return reinterpret_cast<Handle>(h);
}

void ContextKitLayer::removeHandle(Handle handle)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (h)
        delete h;
}

void ContextKitLayer::setProperty(Handle handle, Properties properties)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (!h)
        return;

    if (properties & Publish)
        connect(h, SIGNAL(valueChanged()),
                this, SLOT(contextHandleChanged()));
    else
        disconnect(h, SIGNAL(valueChanged()),
                   this, SLOT(contextHandleChanged()));
}

void ContextKitLayer::contextHandleChanged()
{
    emit handleChanged(reinterpret_cast<Handle>(sender()));
}

bool ContextKitLayer::value(Handle handle, QVariant *data)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (h)
        return h->value("", data);
    else
        return false;
}

bool ContextKitLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (h)
        return h->value(subPath, data);
    else
        return false;
}

bool ContextKitLayer::setValue(QValueSpacePublisher *, Handle handle,
                               const QString &path, const QVariant &value)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (h)
        return h->setValue(path, value);
    else
        return false;
}

bool ContextKitLayer::removeValue(QValueSpacePublisher *, Handle handle,
                                  const QString &path)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (h)
        return h->unsetValue(path);
    else
        return false;
}

bool ContextKitLayer::removeSubTree(QValueSpacePublisher *vsp, Handle handle)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    if (!h)
        return false;

    // this is far from perfect, just removes all values inside this one
    // can't remove sub-trees properly as there's no way to discover them
    foreach (QString kid, h->children())
        if (!h->unsetValue(kid)) return false;

    return true;
}

bool ContextKitLayer::notifyInterest(Handle handle, bool interested)
{
    ContextKitHandle *h = handleToCKHandle(handle);

    if (interested)
        h->subscribe();
    else
        h->unsubscribe();
    return true;
}

QSet<QString> ContextKitLayer::children(Handle handle)
{
    ContextKitHandle *h = handleToCKHandle(handle);
    return h->children();
}

#include "moc_contextkitlayer_p.cpp"

QTM_END_NAMESPACE

