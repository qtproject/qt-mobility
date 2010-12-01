/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "trackerNotifier_signals.h"
#include "trackerNotifier_p.h"

#include <qcoreapplication.h>
#include <qvariant.h>
#include <qstringlist.h>
#include <qvector.h>

#include <qdebug.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

/*
    Allows a metatype to be declared for a type containing commas.
    For example:
	Q_DECLARE_METATYPE_COMMA(QList<QPair<QByteArray,QByteArray> >)
*/
#define Q_DECLARE_METATYPE_COMMA(...) \
    QT_BEGIN_NAMESPACE \
    template <> \
    struct QMetaTypeId< __VA_ARGS__ > \
    { \
        enum { Defined = 1 }; \
        static int qt_metatype_id() \
        { \
            static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
            if (!metatype_id) \
                metatype_id = qRegisterMetaType< __VA_ARGS__ >( #__VA_ARGS__, \
                              reinterpret_cast< __VA_ARGS__ *>(quintptr(-1))); \
            return metatype_id; \
        } \
    }; \
    QT_END_NAMESPACE

Q_DECLARE_METATYPE_COMMA(QMap<QString, QString>)
Q_DECLARE_METATYPE_COMMA(QVector<QMap<QString, QString> >)
Q_DECLARE_METATYPE_COMMA(QVector<QVector<QMap<QString, QString> > >)

Q_DECLARE_METATYPE(Quad)
Q_DECLARE_METATYPE(QVector<Quad>)

QT_BEGIN_NAMESPACE

// How to recognize tracker
static QLatin1String service("org.freedesktop.Tracker1");
static QLatin1String basePath("/org/freedesktop/Tracker1");
static QLatin1String resourcesInterface("org.freedesktop.Tracker1.Resources");
static QLatin1String resourcesPath("/org/freedesktop/Tracker1/Resources");
static QLatin1String changedSignal("GraphUpdated");
static QLatin1String changedSignature("sa(iiii)a(iiii)");

// We'll open only one D-Bus connection per thread; it will be shared between
// QTrackerDriver and QTrackerChangeNotifier.
QThreadStorage<QDBusConnection*> dbusTls;
// how many instances are using the same D-Bus connection (in this thread)
QThreadStorage<int*> dbusRefCountTls;
QAtomicInt connectionCounter = 0;

static QDBusConnection getConnection()
{
    // Create a separate D-Bus connection for each thread. Use
    // ClosingDBusConnection so that the bus gets disconnected when the thread
    // storage is deleted.
    if (!dbusTls.hasLocalData()) {
        QString id = QString::number(connectionCounter.fetchAndAddRelaxed(1))
            .prepend(QString::fromLatin1("landmarks-qsparql-dbus-"));
        dbusTls.setLocalData(new QDBusConnection
                             (QDBusConnection::connectToBus(QDBusConnection::SessionBus, id)));
        dbusRefCountTls.setLocalData(new int(0));
    }
    ++(*dbusRefCountTls.localData());
    return *dbusTls.localData();
}

static void dropConnection()
{
    --(*dbusRefCountTls.localData());
    if (*(dbusRefCountTls.localData()) == 0) {
        QDBusConnection::disconnectFromBus(dbusTls.localData()->name());
        dbusTls.setLocalData(0); // this deletes the connection
        dbusRefCountTls.setLocalData(0);
        // If this thread needs to reconnect, a QDBusConnection with a different
        // name will be created.
    }
}

// D-Bus marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const Quad &t)
{
    argument.beginStructure();
    argument << t.graph << t.subject << t.predicate << t.object;
    argument.endStructure();
    return argument;
}

// D-Bus demarshalling
const QDBusArgument &operator>>(const QDBusArgument &argument, Quad &t)
{
    argument.beginStructure();
    argument >> t.graph >> t.subject >> t.predicate >> t.object;
    argument.endStructure();
    return argument;
}

QTrackerChangeNotifier::QTrackerChangeNotifier(const QString& className,
                                               QObject* parent)
    : QObject(parent)
{
    qDBusRegisterMetaType<Quad>();
    qDBusRegisterMetaType<QVector<Quad> >();
    d = new QTrackerChangeNotifierPrivate(className, getConnection(), this);
}

QTrackerChangeNotifier::~QTrackerChangeNotifier()
{
    dropConnection();
}

QTrackerChangeNotifierPrivate::QTrackerChangeNotifierPrivate(
    const QString& className,
    QDBusConnection c,
    QTrackerChangeNotifier* q)
    : QObject(q), q(q), className(className), connection(c)
{
    // Start listening to the actual signal
    bool ok = connection.connect(service, resourcesPath,
                                 resourcesInterface, changedSignal,
                                 QStringList() << className,
                                 changedSignature,
                                 this, SLOT(changed(QString,
                                                    QVector<Quad>,
                                                    QVector<Quad>)));
    if (!ok) {
        qWarning() << "Cannot connect to signal from Tracker";
    }
}

QString QTrackerChangeNotifier::watchedClass() const
{
    return d->className;
}

void QTrackerChangeNotifierPrivate::changed(QString,
                                            QVector<Quad> deleted,
                                            QVector<Quad> inserted)
{
    // D-Bus will filter based on the class name, so we only get relevant
    // signals here.
    QList<QList<int> > deletes;
    QList<QList<int> > inserts;

    foreach(const Quad& q, deleted) {
        deletes <<
            (QList<int>() << q.graph << q.subject << q.predicate << q.object);
    }

    foreach(const Quad& q, inserted) {
        inserts <<
            (QList<int>() << q.graph << q.subject << q.predicate << q.object);
    }

    emit q->changed(deletes, inserts);
}

QT_END_NAMESPACE
