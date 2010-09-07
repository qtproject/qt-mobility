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

#include "xqcentralrepositoryutils.h"
#include "xqcentralrepositoryutils_p.h"

/*
    \class XQCentralRepositoryUtils
    \internal

    \brief The XQCentralRepositoryUtils implements Central Repository related special
    features in SettingsManager. This class is just a Qt-like wrapper to
    Symbian OS' CRepository class.
    
    See Symbian documentation for more detailed information about e.g. the use of transactions.
    
    This class co-operates with XQSettingsManager so central repositories accessed via this class
    share the same CRepository instances.
*/


/*!
    \enum XQCentralRepositoryUtils::TransactionMode

    This enum defines the possible transaction modes.
*/
/*  \var XQCentralRepositoryUtils::ConcurrentReadWriteTransaction XQCentralRepositoryUtils::ConcurrentReadWriteTransaction
    See Symbian documentation for CRepository transactions.
*/
/*  \var XQCentralRepositoryUtils::ReadTransaction XQCentralRepositoryUtils::ReadTransaction
    See Symbian documentation for CRepository transactions.
*/
/*  \var XQCentralRepositoryUtils::ReadWriteTransaction XQCentralRepositoryUtils::ReadWriteTransaction
    See Symbian documentation for CRepository transactions.
*/

/*
    \enum XQCentralRepositoryUtils::TransactionState

    This enum defines the possible transaction states.
*/
/*  \var XQCentralRepositoryUtils::NotInTransaction XQCentralRepositoryUtils::NotInTransaction
    See Symbian documentation for CRepository transactions.
*/
/*  \var XQCentralRepositoryUtils::ActiveTransaction XQCentralRepositoryUtils::ActiveTransaction
    See Symbian documentation for CRepository transactions.
*/
/*  \var XQCentralRepositoryUtils::FailedTransaction XQCentralRepositoryUtils::FailedTransaction
    See Symbian documentation for CRepository transactions.
*/
/* \var XQCentralRepositoryUtils::UnknownTransaction XQCentralRepositoryUtils::UnknownTransaction
    Transaction state is not known.
*/


/*
    Constructs an XQCentralRepositoryUtils object.
    \param settingsManager Reference to XQSettingsManager instance
*/
XQCentralRepositoryUtils::XQCentralRepositoryUtils(XQSettingsManager& settingsManager, QObject* parent)
    : QObject(parent), d(new XQCentralRepositoryUtilsPrivate(settingsManager))
{
}

/*
    Destructs an XQCentralRepositoryUtils.
*/
XQCentralRepositoryUtils::~XQCentralRepositoryUtils()
{
}

/*
    Creates a key to central repository. (NOTE: This function is not enabled in public
    releases by default.)
    \param key XQCentralRepositorySettingsKey which defines the key details
    \param value Initial value to be written into central repository. The type is determined with
    QVariant.type() and it must be either QVariant::Int, QVariant::Double, QVariant::String or
    QVariant::ByteArray.
    \return True if the key was successfully created, otherwise return false.
*/
bool XQCentralRepositoryUtils::createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value)
{
    return d->createKey(key, value);
}

/*
    Deletes a key from central repository. (NOTE: This function is not enabled in public
    releases by default.)
    \param key XQCentralRepositorySettingsKey which defines the key details
    \return True if the key was successfully deleted, otherwise return false.
*/
bool XQCentralRepositoryUtils::deleteKey(const XQCentralRepositorySettingsKey& key)
{
    return d->deleteKey(key);
}

/*
    Resets a key in central repository to it's initial value defined in .ini file.
    \param key XQCentralRepositorySettingsKey which defines the key details
    \return True if the key was successfully reset, otherwise return false.
*/
bool XQCentralRepositoryUtils::resetKey(const XQCentralRepositorySettingsKey& key)
{
    return d->resetKey(key);
}

/*
    Resets the whole central repository file to it's initial state defined in .ini file.
    \param repositoryUid Repository UID
    \return True if the repository was successfully reset, otherwise return false.
*/
bool XQCentralRepositoryUtils::resetRepository(long int repositoryUid)
{
    return d->resetRepository(repositoryUid);
}

/*
    Finds keys in central repository.
    \param criteria Search criteria
    \return List of XQCentralRepositorySettingsKey objects that match with criteria
*/
QList<XQCentralRepositorySettingsKey> XQCentralRepositoryUtils::findKeys(const XQCentralRepositorySearchCriteria& criteria)
{
    return d->findKeys(criteria);
}

/*
    Starts transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
    \param transactionMode Transaction mode.
    \return True if the transaction was successfully started, otherwise return false.
*/
bool XQCentralRepositoryUtils::startTransaction(long int repositoryUid, TransactionMode transactionMode)
{
    return d->startTransaction(repositoryUid, transactionMode);
}

/*
    Commits transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
    \return True if the transaction was successfully committed, otherwise return false.
*/
bool XQCentralRepositoryUtils::commitTransaction(long int repositoryUid)
{
    return d->commitTransaction(repositoryUid);
}

/*
    Cancels transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
*/
void XQCentralRepositoryUtils::cancelTransaction(long int repositoryUid)
{
    d->cancelTransaction(repositoryUid);
}

/*
    Fails transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
*/
void XQCentralRepositoryUtils::failTransaction(long int repositoryUid)
{
    d->failTransaction(repositoryUid);
}

/*
    \return Current transaction state.
*/
XQCentralRepositoryUtils::TransactionState XQCentralRepositoryUtils::transactionState(long int repositoryUid) const
{
    return d->transactionState(repositoryUid);
}
