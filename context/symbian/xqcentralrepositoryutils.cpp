#include "xqcentralrepositoryutils.h"
#include "xqcentralrepositoryutils_p.h"

/*!
    \class XQCentralRepositoryUtils

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
/*! \var XQCentralRepositoryUtils::ConcurrentReadWriteTransaction XQCentralRepositoryUtils::ConcurrentReadWriteTransaction
    See Symbian documentation for CRepository transactions.
*/
/*! \var XQCentralRepositoryUtils::ReadTransaction XQCentralRepositoryUtils::ReadTransaction
    See Symbian documentation for CRepository transactions.
*/
/*! \var XQCentralRepositoryUtils::ReadWriteTransaction XQCentralRepositoryUtils::ReadWriteTransaction
    See Symbian documentation for CRepository transactions.
*/

/*!
    \enum XQCentralRepositoryUtils::TransactionState

    This enum defines the possible transaction states.
*/
/*! \var XQCentralRepositoryUtils::NotInTransaction XQCentralRepositoryUtils::NotInTransaction
    See Symbian documentation for CRepository transactions.
*/
/*! \var XQCentralRepositoryUtils::ActiveTransaction XQCentralRepositoryUtils::ActiveTransaction
    See Symbian documentation for CRepository transactions.
*/
/*! \var XQCentralRepositoryUtils::FailedTransaction XQCentralRepositoryUtils::FailedTransaction
    See Symbian documentation for CRepository transactions.
*/
/*! \var XQCentralRepositoryUtils::UnknownTransaction XQCentralRepositoryUtils::UnknownTransaction
    Transaction state is not known.
*/


/*!
    Constructs an XQCentralRepositoryUtils object.
    \param settingsManager Reference to XQSettingsManager instance
*/
XQCentralRepositoryUtils::XQCentralRepositoryUtils(XQSettingsManager& settingsManager, QObject* parent)
    : QObject(parent), d(new XQCentralRepositoryUtilsPrivate(settingsManager))
{
}

/*!
    Destructs an XQCentralRepositoryUtils.
*/
XQCentralRepositoryUtils::~XQCentralRepositoryUtils()
{
}

/*!
    Creates a key to central repository. (NOTE: This function is not enabled in public
    releases by default.)
    \param key XQCentralRepositorySettingsKey which defines the key details
    \param value Initial value to be written into central repository. The type is determined with
    QVariant.type() and it must be either QVariant::Int, QVariant::Double, QVariant::String or
    QVariant::ByteArray.
    \return True if the key was succesfully created, otherwise return false.
*/
bool XQCentralRepositoryUtils::createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value)
{
    return d->createKey(key, value);
}

/*!
    Deletes a key from central repository. (NOTE: This function is not enabled in public
    releases by default.)
    \param key XQCentralRepositorySettingsKey which defines the key details
    \return True if the key was succesfully deleted, otherwise return false.
*/
bool XQCentralRepositoryUtils::deleteKey(const XQCentralRepositorySettingsKey& key)
{
    return d->deleteKey(key);
}

/*!
    Resets a key in central repository to it's initial value defined in .ini file.
    \param key XQCentralRepositorySettingsKey which defines the key details
    \return True if the key was succesfully reset, otherwise return false.
*/
bool XQCentralRepositoryUtils::resetKey(const XQCentralRepositorySettingsKey& key)
{
    return d->resetKey(key);
}

/*!
    Resets the whole central repository file to it's initial state defined in .ini file.
    \param repositoryUid Repository UID
    \return True if the repository was succesfully reset, otherwise return false.
*/
bool XQCentralRepositoryUtils::resetRepository(long int repositoryUid)
{
    return d->resetRepository(repositoryUid);
}

/*!
    Finds keys in central repository.
    \param criteria Search criteria
    \return List of XQCentralRepositorySettingsKey objects that match with criteria
*/
QList<XQCentralRepositorySettingsKey> XQCentralRepositoryUtils::findKeys(const XQCentralRepositorySearchCriteria& criteria)
{
    return d->findKeys(criteria);
}

/*!
    Starts transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
    \param transactionMode Transaction mode.
    \return True if the transaction was succesfully started, otherwise return false.
*/
bool XQCentralRepositoryUtils::startTransaction(long int repositoryUid, TransactionMode transactionMode)
{
    return d->startTransaction(repositoryUid, transactionMode);
}

/*!
    Commits transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
    \return True if the transaction was succesfully committed, otherwise return false.
*/
bool XQCentralRepositoryUtils::commitTransaction(long int repositoryUid)
{
    return d->commitTransaction(repositoryUid);
}

/*!
    Cancels transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
*/
void XQCentralRepositoryUtils::cancelTransaction(long int repositoryUid)
{
    d->cancelTransaction(repositoryUid);
}

/*!
    Fails transaction. See Symbian documentation for more info.
    \param repositoryUid Repository UID
*/
void XQCentralRepositoryUtils::failTransaction(long int repositoryUid)
{
    d->failTransaction(repositoryUid);
}

/*!
    \return Current transaction state.
*/
XQCentralRepositoryUtils::TransactionState XQCentralRepositoryUtils::transactionState(long int repositoryUid) const
{
    return d->transactionState(repositoryUid);
}
