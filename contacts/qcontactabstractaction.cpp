#include "qcontactabstractaction.h"
#include "qcontactabstractaction_p.h"

QContactAbstractAction::QContactAbstractAction()
{
}

QContactAbstractAction::~QContactAbstractAction()
{
}

QString QContactAbstractAction::actionName() const
{
    return d->m_actionName;
}

QString QContactAbstractAction::vendor() const
{
    return d->m_vendor;
}

int QContactAbstractAction::implementationVersion() const
{
    return d->m_implementationVersion;
}

QVariantMap QContactAbstractAction::metadata() const
{
    return d->m_metadata;
}

QContactActionFilter QContactAbstractAction::contactFilter() const
{
    return d->m_filter;
}

bool QContactAbstractAction::supportsDetail(const QContactDetail& detail) const
{
    Q_UNUSED(detail);
    return false;
}

QList<QContactDetail> QContactAbstractAction::supportedDetails(const QContact& contact) const
{
    Q_UNUSED(contact);
    return QList<QContactDetail>();
}
