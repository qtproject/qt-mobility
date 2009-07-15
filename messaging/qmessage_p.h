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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessage.h"

class QMessagePrivate
{
    Q_DECLARE_PUBLIC(QMessage)

public:
    QMessagePrivate(QMessage *message)
        :q_ptr(message)
    {
    }

    void setId(const QMessageId &id);
    void setDataModified(bool modified);
    void setParentAccountId(const QMessageAccountId &accountId);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    void setParentFolderId(const QMessageFolderId &folderId);
#endif
    void setSize(uint size);

    QMessage *q_ptr;
};

/*
    Sets the identifier of the message to \a id.
    
    \sa id()
*/
void QMessagePrivate::setId(const QMessageId &id)
{
    Q_UNUSED(id)
}

/*
    Sets the modified data state of the message to \a modified.
*/
void QMessagePrivate::setDataModified(bool modified)
{
    Q_UNUSED(modified);
}

/*
    Sets the identifier of the parent account of the message to \a accountId.
*/
void QMessagePrivate::setParentAccountId(const QMessageAccountId &accountId)
{
    Q_UNUSED(accountId)
}

/*!
    Sets the identifier of the folder that contains the message to \a folderId.
*/
#ifdef QMESSAGING_OPTIONAL_FOLDER
void QMessagePrivate::setParentFolderId(const QMessageFolderId &folderId)
{
    Q_UNUSED(folderId)
}
#endif

/*
    Sets the complete size of the message as found on the originating server to \a size.
    
    \sa size()
*/
void QMessagePrivate::setSize(uint size)
{
    Q_UNUSED(size)
}
