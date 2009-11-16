#include "xqpublishandsubscribeutils.h"
#include "xqpublishandsubscribeutils_p.h"

/*!
    \class XQPublishAndSubscribeUtils

    \brief The XQPublishAndSubscribeUtils implements Publish and Subscribe related special
    features in SettingsManager. This class is just a Qt-like wrapper to
    Symbian OS' RPropery class.
*/


/*!
    Constructs an XQPublishAndSubscribeUtils object.
    \param settingsManager Reference to XQSettingsManager instance
*/
XQPublishAndSubscribeUtils::XQPublishAndSubscribeUtils(XQSettingsManager& settingsManager, QObject* parent)
    : QObject(parent), d(new XQPublishAndSubscribeUtilsPrivate(settingsManager))
{
}

/*!
    Destructs an XQPublishAndSubscribeUtils object.
*/
XQPublishAndSubscribeUtils::~XQPublishAndSubscribeUtils()
{
}

/*!
    Defines a propery
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \param type Propery type (NOTE: TypeDouble is not supported by Publish and Subscribe.)
    \return True if the property was succesfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type)
{
    return d->defineProperty(key, type);
}

/*!
    Defines a propery with security policies
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \param type Propery type (NOTE: TypeDouble is not supported by Publish and Subscribe.)
    \param readPolicy Policy definition for read operations
    \param writePolicy Policy definition for write operations
    \return True if the property was succesfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
    const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy)
{
    return d->defineProperty(key, type, readPolicy, writePolicy);
}

/*!
    Deletes a propery
    \param key XQPublishAndSubscribeSettingsKey which defines the propery details
    \return True if the property was succesfully defined, otherwise return false.
*/
bool XQPublishAndSubscribeUtils::deleteProperty(const XQPublishAndSubscribeSettingsKey& key)
{
    return d->deleteProperty(key);
}
