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
#include "qmessagefolder.h"
#include "qmfhelpers_p.h"

#include <qmailfolder.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);


QTM_BEGIN_NAMESPACE

class QMessageFolderPrivate
{
public:
    QMailFolder _folder;

    static QMessageFolder convert(const QMailFolder &folder);
    //static QMailFolder convert(const QMessageFolder &folder);
};

QMessageFolder QMessageFolderPrivate::convert(const QMailFolder &folder)
{
    QMessageFolder result;
    result.d_ptr->_folder = folder;
    return result;
}

/*
QMailFolder QMessageFolderPrivate::convert(const QMessageFolder &folder)
{
    return folder.d_ptr->_folder;
}
*/

namespace QmfHelpers {

QMessageFolder convert(const QMailFolder &folder)
{
    return QMessageFolderPrivate::convert(folder);
}

/*
QMailFolder convert(const QMessageFolder &folder)
{
    return QMessageFolderPrivate::convert(folder);
}
*/

}

QMessageFolder::QMessageFolder()
    : d_ptr(new QMessageFolderPrivate)
{
}

QMessageFolder::QMessageFolder(const QMessageFolderId &id)
    : d_ptr(new QMessageFolderPrivate)
{
    *this = QMessageManager().folder(id);
}

QMessageFolder::QMessageFolder(const QMessageFolder &other)
    : d_ptr(new QMessageFolderPrivate)
{
    this->operator=(other);
}

QMessageFolder& QMessageFolder::operator=(const QMessageFolder& other)
{
    if (&other != this) {
        d_ptr->_folder = other.d_ptr->_folder;
    }

    return *this;
}

QMessageFolder::~QMessageFolder()
{
    delete d_ptr;
}

QMessageFolderId QMessageFolder::id() const
{
    return convert(d_ptr->_folder.id());
}

QMessageAccountId QMessageFolder::parentAccountId() const
{
    return convert(d_ptr->_folder.parentAccountId());
}

QMessageFolderId QMessageFolder::parentFolderId() const
{
    return convert(d_ptr->_folder.parentFolderId());
}

QString QMessageFolder::name() const
{
    return d_ptr->_folder.displayName();
}

QString QMessageFolder::path() const
{
    return d_ptr->_folder.path();
}

QTM_END_NAMESPACE
