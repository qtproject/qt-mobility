/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "qgallerytrackerfileeditresponse_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qthread.h>
#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerFileEditResponseThread : public QThread
{
public:
    QGalleryTrackerFileEditResponseThread(QGalleryTrackerFileEditResponse *response)
        : m_response(response)
    {
    }

protected:
    void run()
    {
        m_response->run();
    }

private:
    QGalleryTrackerFileEditResponse *m_response;
};

QGalleryTrackerFileEditResponse::QGalleryTrackerFileEditResponse(
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QGalleryTrackerSchema &schema,
        const QStringList &properties,
        const QStringList &fileNames,
        QObject *parent)
    : QGalleryAbstractResponse(parent)
    , idFunc(schema.idFunc())
    , m_currentIndex(-1)
    , m_error(0)
    , m_cancelled(0)
    , m_call(0)
    , m_thread(0)
    , m_metaDataInterface(metaDataInterface)
{
    QStringList fields;

    fields.append(QLatin1String("File:Path"));
    fields.append(QLatin1String("File:Name"));

    m_propertyNames.append(QLatin1String("filePath"));
    m_propertyNames.append(QLatin1String("fileName"));

    for (QStringList::const_iterator property = properties.begin(), end = properties.end();
            property != end;
            ++property) {
        QString field = schema.field(*property);

        if (!field.isEmpty() && !fields.contains(field)) {
            fields.append(field);
            m_propertyNames.append(*property);
        }
    }

    m_call = new QDBusPendingCallWatcher(m_metaDataInterface->asyncCall(
            QLatin1String("GetMultiple"),
            QLatin1String("Files"),
            fileNames,
            fields), this);

    if (m_call->isFinished()) {
        callFinished(m_call);
    } else {
        connect(m_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(callFinished(QDBusPendingCallWatcher*)));
    }
}

QGalleryTrackerFileEditResponse::~QGalleryTrackerFileEditResponse()
{

}

int QGalleryTrackerFileEditResponse::minimumPagedItems() const
{
    return 1;
}

QStringList QGalleryTrackerFileEditResponse::propertyNames() const
{
    return m_propertyNames;
}

int QGalleryTrackerFileEditResponse::propertyKey(const QString &name) const
{
    return m_propertyNames.indexOf(name);
}

QGalleryProperty::Attributes QGalleryTrackerFileEditResponse::propertyAttributes(int) const
{
    return QGalleryProperty::CanRead;
}

int QGalleryTrackerFileEditResponse::count() const
{
    return m_currentIndex != -1 ? 1 : 0;
}

QString QGalleryTrackerFileEditResponse::id(int index) const
{
    return m_currentIndex != -1 && index == 0
        ? idFunc(m_rows.at(m_currentIndex))
        : QString();
}

QUrl QGalleryTrackerFileEditResponse::url(int index) const
{
    QUrl url;

    if (m_currentIndex != -1 && index == 0) {
        url.setScheme(QLatin1String("file"));
        url.setPath(m_rows.at(m_currentIndex).at(0));
    }

    return url;
}

QString QGalleryTrackerFileEditResponse::type(int index) const
{
    return m_currentIndex != -1 && index == 0
            ? QGalleryTrackerSchema::typeFromService(m_rows.at(m_currentIndex).at(1))
            : QString();
}

QList<QGalleryResource> QGalleryTrackerFileEditResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerFileEditResponse::status(int) const
{
    return ItemStatus();
}

QVariant QGalleryTrackerFileEditResponse::metaData(int index, int key) const
{
    return m_currentIndex != -1 && index == 0
        ? m_rows.at(m_currentIndex).value(key)
        : QString();
}

void QGalleryTrackerFileEditResponse::setMetaData(int, int, const QVariant &)
{
}

void QGalleryTrackerFileEditResponse::cancel()
{
    if (m_call || m_thread)
        m_cancelled = true;
    else
        QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerFileEditResponse::waitForFinished(int msecs)
{
    if (m_call) {
        m_call->waitForFinished();

        callFinished(m_call);
    }

    if (m_thread) {
        return m_thread->wait(msecs);
    } else {
        return true;
    }
}

void QGalleryTrackerFileEditResponse::customEvent(QEvent *event)
{
    if (event->type() == QEvent::User) {
        const int index = m_currentIndex + 1;

        if (m_currentIndex != -1) {
            m_currentIndex = -1;

            emit removed(0, 1);
        }

        if (index < m_rows.count()) {
            m_currentIndex = index;

            emit inserted(0, 1);
        }
    }
}

void QGalleryTrackerFileEditResponse::callFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher != m_call) {
        // This shouldn't ever happen.
        Q_ASSERT(false);

        return;
    }
    m_call = 0;

    watcher->deleteLater();

    QDBusPendingReply<QVector<QStringList> > reply(*watcher);

    if (reply.isError()) {
        qWarning("DBUS error %s", qPrintable(reply.error().message()));

        finish(QGalleryAbstractRequest::ConnectionError);
    } else if (m_cancelled) {
        QGalleryAbstractResponse::cancel();
    } else {
        m_rows = reply.value();

        if (!m_rows.isEmpty()) {
            m_thread = new QGalleryTrackerFileEditResponseThread(this);

            connect(m_thread, SIGNAL(finished()), this, SLOT(threadFinished()));

            m_thread->start();
        } else {
            finish(QGalleryAbstractRequest::Succeeded);
        }
    }
}

void QGalleryTrackerFileEditResponse::threadFinished()
{
    m_thread->deleteLater();
    m_thread = 0;

    if (m_currentIndex >= 0) {
        m_currentIndex = -1;

        removed(0, 1);
    }

    m_rows.clear();

    finish(m_error == 0 ? QGalleryAbstractRequest::Succeeded : m_error);
}

void QGalleryTrackerFileEditResponse::run()
{
    int index = 0;

    for (QVector<QStringList>::const_iterator it = m_rows.constBegin(), end = m_rows.constEnd();
            !m_cancelled && it != end;
            ++it, ++index) {
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));

        const QString path = m_rows.at(index).value(0);
        const QString fileName = m_rows.at(index).value(1);

        if (fileName.isEmpty()) //
            continue;

        if (!editFile(&m_error, path, fileName))
            return;
    }
}

#include "moc_qgallerytrackerfileeditresponse_p.cpp"

QTM_END_NAMESPACE
