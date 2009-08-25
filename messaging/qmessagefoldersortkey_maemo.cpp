/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefoldersortkey.h"
#include "qmessagefoldersortkey_p.h"


QMessageFolderSortKey::QMessageFolderSortKey()
{
}

QMessageFolderSortKey::QMessageFolderSortKey(const QMessageFolderSortKey &other)
{
    Q_UNUSED(other)
}

QMessageFolderSortKey::~QMessageFolderSortKey()
{
}

bool QMessageFolderSortKey::isEmpty() const
{
    return false; // stub
}

bool QMessageFolderSortKey::isSupported() const
{
    return true; // stub
}

QMessageFolderSortKey QMessageFolderSortKey::operator+(const QMessageFolderSortKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderSortKey(); // stub
}

QMessageFolderSortKey& QMessageFolderSortKey::operator+=(const QMessageFolderSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageFolderSortKey::operator==(const QMessageFolderSortKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

const QMessageFolderSortKey& QMessageFolderSortKey::operator=(const QMessageFolderSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageFolderSortKey QMessageFolderSortKey::displayName(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageFolderSortKey(); // stub
}

QMessageFolderSortKey QMessageFolderSortKey::path(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageFolderSortKey(); // stub
}
#endif
