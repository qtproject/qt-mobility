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

#ifndef QMDEGALLERYRESULTSET_H
#define QMDEGALLERYRESULTSET_H

#include <qmobilityglobal.h>
#include "qgalleryresultset.h"

#include <QtCore/qobject.h>
#include <qurl.h>
#include <qvariant.h>

#include <e32cmn.h>

class CMdEObject;

QTM_BEGIN_NAMESPACE

class QMdeSession;

class QMDEGalleryResultSet : public QGalleryResultSet
{
    Q_OBJECT
public:
    QMDEGalleryResultSet(QObject *parent = 0);

    ~QMDEGalleryResultSet();

    virtual int propertyKey(const QString &property) const;
    virtual QGalleryProperty::Attributes propertyAttributes(int key) const;
    virtual QVariant::Type propertyType(int key) const;

    virtual int itemCount() const;
    // non pure virtual
    virtual bool isValid() const;

    virtual QVariant itemId() const;
    virtual QUrl itemUrl() const;
    virtual QString itemType() const;
    // non pure virtual
    virtual QList<QGalleryResource> resources() const;

    virtual QVariant metaData(int key) const;
    virtual bool setMetaData(int key, const QVariant &value);

    virtual int currentIndex() const;
    virtual bool fetch(int index);
    /* non pure virtual */
    virtual bool fetchNext();
    virtual bool fetchPrevious();
    virtual bool fetchFirst();
    virtual bool fetchLast();

public: //MDS spesific

    virtual void createQuery() = 0;

    // Signals to send
    //        void currentIndexChanged(int index);
    //        void currentItemChanged();
    //        void itemsInserted(int index, int count);
    //        void itemsRemoved(int index, int count);
    //
    //        void metaDataChanged(int index, int count, const QList<int> &keys);

protected:
    QMdeSession *m_session;

protected:

    int m_cursorPosition;
    bool m_live;
    int m_result;
    QString m_itemType;
    bool m_isValid;

    RPointerArray<CMdEObject> m_itemArray;

};

QTM_END_NAMESPACE
#endif // QMDEGALLERYRESULTSET_H
