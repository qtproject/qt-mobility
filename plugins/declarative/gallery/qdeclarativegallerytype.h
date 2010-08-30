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

#ifndef QDECLARATIVEGALLERYTYPE_H
#define QDECLARATIVEGALLERYTYPE_H

#include <qgallerytyperequest.h>

#include <QtCore/qpointer.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativepropertymap.h>

#include "qdeclarativegalleryfilter.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryType : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(State)
    Q_ENUMS(Result)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery NOTIFY galleryChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(Result result READ result NOTIFY resultChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(bool live READ isLive WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(QObject *metaData READ metaData NOTIFY metaDataChanged)
public:
    enum State
    {
        Inactive    = QGalleryAbstractRequest::Inactive,
        Active      = QGalleryAbstractRequest::Active,
        Cancelling  = QGalleryAbstractRequest::Cancelling,
        Idle        = QGalleryAbstractRequest::Idle
    };

    enum Result
    {
        NoResult                        = QGalleryAbstractRequest::NoResult,
        Succeeded                       = QGalleryAbstractRequest::NoResult,
        Cancelled                       = QGalleryAbstractRequest::NoResult,
        NoGallery                       = QGalleryAbstractRequest::NoResult,
        NotSupported                    = QGalleryAbstractRequest::NoResult,
        ConnectionError                 = QGalleryAbstractRequest::NoResult,
        InvalidItemError                = QGalleryAbstractRequest::NoResult
    };

    QDeclarativeGalleryType(QObject *parent = 0);
    ~QDeclarativeGalleryType();

    QAbstractGallery *gallery() const { return m_request.gallery(); }
    void setGallery(QAbstractGallery *gallery) {
        if (!m_complete || !gallery) { m_request.setGallery(gallery); emit galleryChanged(); } }

    State state() const { return State(m_request.state()); }
    Result result() const { return Result(m_request.result()); }

    int currentProgress() const { return m_request.currentProgress(); }
    int maximumProgress() const { return m_request.maximumProgress(); }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setPropertyNames(names); emit propertyNamesChanged(); }

    bool isLive() const { return m_request.isLive(); }
    void setLive(bool live) { m_request.setLive(live); emit liveChanged(); }

    QString itemType() const { return m_request.itemType(); }
    void setItemType(const QString itemType)
    {
        m_request.setItemType(itemType);
        if (m_complete)
            m_request.execute();
        emit itemTypeChanged();
    }

    bool available() const { return m_request.isValid(); }

    QObject *metaData() const { return m_metaData; }

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void reload() { m_request.execute(); }
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged();
    void resultChanged();
    void progressChanged();
    void availableChanged();
    void metaDataChanged();

    void galleryChanged();
    void propertyNamesChanged();
    void liveChanged();
    void itemTypeChanged();

private Q_SLOTS:
    void _q_typeChanged();
    void _q_metaDataChanged(const QList<int> &keys);

private:
    QGalleryTypeRequest m_request;
    QDeclarativePropertyMap *m_metaData;
    QHash<int, QString> m_propertyKeys;
    bool m_complete;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryType))

#endif
