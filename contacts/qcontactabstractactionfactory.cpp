#include "qcontactabstractactionfactory.h"
#include "qcontactabstractaction.h"

QContactAbstractActionFactory::QContactAbstractActionFactory()
{
}

QContactAbstractActionFactory::~QContactAbstractActionFactory()
{
}

QStringList QContactAbstractActionFactory::actionNames()
{
    return QStringList();
}

QContactAbstractAction* QContactAbstractActionFactory::instance(const QString& actionName, const QString& vendor, int implementationVersion)
{
    Q_UNUSED(actionName);
    Q_UNUSED(vendor);
    Q_UNUSED(implementationVersion);
    return 0;
}

QList<QContactAbstractAction*> QContactAbstractActionFactory::instances(const QString& actionName, const QString& vendor, int implementationVersion)
{
    Q_UNUSED(actionName);
    Q_UNUSED(vendor);
    Q_UNUSED(implementationVersion);
    return QList<QContactAbstractAction*>();
}
