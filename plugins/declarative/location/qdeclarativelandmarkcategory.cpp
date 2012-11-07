/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
***************************************************************************/

#include "qdeclarativelandmarkcategory_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass LandmarkCategory QDeclarativeLandmarkCategory
    \brief The LandmarkCategory element presents one landmark category.
    \ingroup qml-location-landmarks
    \since Mobility 1.2

    This element is part of the \bold{QtMobility.location 1.1} module.

    The primary source for categories is the \l LandmarkCategoryModel.
    To get \l Landmarks belonging to certain category(ies),
    \l LandmarkCategoryFilter may be used. Currently saving of categories
    is not supported.

    \snippet doc/src/snippets/declarative/declarative-landmark.qml User declared category

    \sa Landmark, LandmarkModel, LandmarkCategoryModel, {QLandmarkCategory}
*/


QDeclarativeLandmarkCategory::QDeclarativeLandmarkCategory(QObject* parent) :
        QObject(parent)
{
}

QString QDeclarativeLandmarkCategory::name()
{
    return m_category.name();
}

/*!
  \qmlproperty string LandmarkCategory::name
  \since Mobility 1.2

  This property holds the name of the category.

  */

void QDeclarativeLandmarkCategory::setName(const QString& name)
{
    if (name == m_category.name())
        return;
    m_category.setName(name);
    emit nameChanged();
}

QUrl QDeclarativeLandmarkCategory::iconSource()
{
    return m_category.iconUrl();
}

/*!
  \qmlproperty url LandmarkCategory::iconSource
  \since Mobility 1.2

  This property holds the icon source URL of the category.

  */

void QDeclarativeLandmarkCategory::setIconSource(const QUrl& iconSource)
{
    if (iconSource == m_category.iconUrl())
        return;
    m_category.setIconUrl(iconSource);
    emit iconSourceChanged();
}

QLandmarkCategory QDeclarativeLandmarkCategory::category() const
{
    return m_category;
}

// Initializes this category from the given category
void QDeclarativeLandmarkCategory::setCategory(const QLandmarkCategory& category)
{
    setName(category.name());
    setIconSource(category.iconUrl());
    m_category = category;
}

#include "moc_qdeclarativelandmarkcategory_p.cpp"

QTM_END_NAMESPACE
