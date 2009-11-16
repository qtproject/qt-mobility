#ifndef XQPUBLISHANDSUBSCRIBESECURITYPOLICY_H
#define XQPUBLISHANDSUBSCRIBESECURITYPOLICY_H

#include "settingsmanager_global.h"

#include <QList>

class XQSETTINGSMANAGER_EXPORT XQPublishAndSubscribeSecurityPolicy
{
public:

    class SecureId
    {
    public:
        SecureId(long int uid);
        inline long int uid() const {return m_uid;};

    private:
        long int m_uid;
    };

    class VendorId
    {
    public:
        VendorId(long int uid);
        inline long int uid() const {return m_uid;};

    private:
        long int m_uid;
    };

    enum SecPolicyType
    {
        SecPolicyAlwaysFail,
        SecPolicyAlwaysPass,
        SecPolicyUndefined
    };
    
    enum Capability
    {
        CapabilityTCB,
        CapabilityCommDD,
        CapabilityPowerMgmt,
        CapabilityMultimediaDD,
        CapabilityReadDeviceData,
        CapabilityWriteDeviceData,
        CapabilityDRM,
        CapabilityTrustedUI,
        CapabilityProtServ,
        CapabilityDiskAdmin,
        CapabilityNetworkControl,
        CapabilityAllFiles,
        CapabilitySwEvent,
        CapabilityNetworkServices,
        CapabilityLocalServices,
        CapabilityReadUserData,
        CapabilityWriteUserData,
        CapabilityLocation,
        CapabilitySurroundingsDD,
        CapabilityUserEnvironment
    };

    XQPublishAndSubscribeSecurityPolicy(SecPolicyType secPolicyType);
    XQPublishAndSubscribeSecurityPolicy(QList<Capability> capabilities);
    XQPublishAndSubscribeSecurityPolicy(SecureId secureId, QList<Capability> capabilities);
    XQPublishAndSubscribeSecurityPolicy(VendorId vendorId, QList<Capability> capabilities);
    ~XQPublishAndSubscribeSecurityPolicy();

    SecPolicyType secPolicyType() const;
    SecureId secureId() const;
    VendorId vendorId() const;
    QList<Capability> capabilities() const;

private:
    SecPolicyType m_secPolicyType;
    SecureId m_secureId;
    VendorId m_vendorId;
    QList<Capability> m_capabilities;
};

#endif //XQPUBLISHANDSUBSCRIBESECURITYPOLICY_H
