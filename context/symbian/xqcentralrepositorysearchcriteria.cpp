#include "xqcentralrepositorysearchcriteria.h"

/*!
    \class XQCentralRepositorySearchCriteria

    \brief The XQCentralRepositorySearchCriteria defines search criteria
    used in XQCentralRepositoryUtils::findKeys().
*/


/*!
    Constructs an XQCentralRepositorySearchCriteria object.
    \param repositoryUid Central repository where the key is searched
    \param partialKey Partial Central repository key
    \param bitMask Bit mask for partialKey
*/
XQCentralRepositorySearchCriteria::XQCentralRepositorySearchCriteria(long int repositoryUid,
    unsigned long int partialKey, unsigned long int bitMask)
    : m_repositoryUid(repositoryUid), m_partialKey(partialKey), m_bitMask(bitMask),
    m_negativeCriteria(false)
{
}

/*!
    Destructs an XQCentralRepositorySearchCriteria object.
*/
XQCentralRepositorySearchCriteria::~XQCentralRepositorySearchCriteria()
{
}

/*!
    Sets the value criteria. If this function is not called or value is NULL object
    every key in the repository is returned in XQCentralRepositoryUtils::findKeys().
    \param value Value to be searched. The type is determined with
    QVariant.type() and it must be either QVariant::Int, QVariant::Double,
    QVariant::String or QVariant::ByteArray.
    \param negativeCriteria If true the keys of which values don't match will be returned in
    XQCentralRepositoryUtils::findKeys()
*/
void XQCentralRepositorySearchCriteria::setValueCriteria(const QVariant& value, bool negativeCriteria)
{
    m_value = value;
    m_negativeCriteria = negativeCriteria;
}

/*!
    \return Repository UID
*/
long int XQCentralRepositorySearchCriteria::repositoryUid() const
{
    return m_repositoryUid;
}

/*!
    \return Partial key
*/
unsigned long int XQCentralRepositorySearchCriteria::partialKey() const
{
    return m_partialKey;
}

/*!
    \return Bit mask for the partial key
*/
unsigned long int XQCentralRepositorySearchCriteria::bitMask() const
{
    return m_bitMask;
}

/*!
    \return Value criteria
*/
const QVariant& XQCentralRepositorySearchCriteria::value() const
{
    return m_value;
}

/*!
    \return True if value criteria is negative
*/
bool XQCentralRepositorySearchCriteria::negativeCriteria() const
{
    return m_negativeCriteria;
}
