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

#include "qmdegalleryresultset.h"
//Backend
#include "qmdesession.h"
//Symbian
#include <mdeproperty.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>

QTM_BEGIN_NAMESPACE

QMDEGalleryResultSet::QMDEGalleryResultSet(QObject *parent)
: QGalleryResultSet(parent)
, m_cursorPosition(0)
{
    // TODO: RArray should be map so that key based find is easier?
    m_itemArray.Reset();
    // create a query session, provide this pointer to take create of deletion of m_session
    m_session = new QMdeSession(this);

}

QMDEGalleryResultSet::~QMDEGalleryResultSet()
{
    m_itemArray.ResetAndDestroy();
}

QUrl QMDEGalleryResultSet::url(int index) const
{

}
int QMDEGalleryResultSet::propertyKey(const QString &property) const
{

}
QGalleryProperty::Attributes QMDEGalleryResultSet::propertyAttributes(int key) const
{

}
QVariant::Type QMDEGalleryResultSet::propertyType(int key) const
{

}

int QMDEGalleryResultSet::itemCount() const
{

}

QVariant QMDEGalleryResultSet::itemId() const
{

}
QUrl QMDEGalleryResultSet::itemUrl() const
{

}
QString QMDEGalleryResultSet::itemType() const
{

}
QVariant QMDEGalleryResultSet::metaData(int key) const
{

}
bool QMDEGalleryResultSet::setMetaData(int key, const QVariant &value)
{
    // TODO - should this save data instantly to mds database?
}
int QMDEGalleryResultSet::currentIndex() const
{

}
bool QMDEGalleryResultSet::fetch(int index)
{

}

QVariant QMDEGalleryResultSet::metaData(int key, const QString& property) const
{
    //TODO:: remove replace old index based metada function. Use Map?
    QVariant retval;

    //    CMdEObject* item = m_itemArray[index];
    //    if (item) {
    //        RBuf buf;
    //        //        TRAP_IGNORE(
    //        CleanupClosePushL(buf);
    //        buf.CreateL(property.length());
    //        buf.Copy(property.utf16(), property.length());
    //        CMdEPropertyDef& mdsPropertyDef = item->Def().GetPropertyDefL(buf);
    //        CMdEProperty* mdsProperty;
    //        TInt propertyIndex = item->Property(mdsPropertyDef, mdsProperty);
    //        if (KErrNotFound != propertyIndex) {
    //            //retval = static_cast<CMdEUint16Property*>(mdsProperty)->Value();
    //            const TDesC& descr = static_cast<CMdETextProperty*> (mdsProperty)->Value();
    //            QString qString((QChar*) descr.Ptr(), descr.Length());
    //            retval = QVariant(qString);
    //        }
    //        CleanupStack::PopAndDestroy(&buf);
    //        //        );
    //    }
    //
    //    return retval;
}

int QMDEGalleryResultSet::minimumPagedItems() const
{

}

void QMDEGalleryResultSet::appendItem(CMdEObject* item)
{

}
#include "moc_qmdegalleryresultset.cpp"
QTM_END_NAMESPACE
