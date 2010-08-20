
#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include "qdeclarativemessagefilter.h"
#include "qdeclarativemessagemodel.h"

class QDeclarativeMessageModelPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.messaging"));
        qmlRegisterType<QDeclarativeMessageModel>(uri, 1, 1, "MessageModel");
        qmlRegisterType<QDeclarativeMessageIntersectionFilter>(uri, 1, 1, "MessageIntersectionFilter");
        qmlRegisterType<QDeclarativeMessageUnionFilter>(uri, 1, 1, "MessageUnionFilter");
        qmlRegisterType<QDeclarativeMessageFilter>(uri, 1, 1, "MessageFilter");
        qmlRegisterUncreatableType<QDeclarativeMessageFilterBase>(uri,1,1,"MessageFilterBase",QDeclarativeMessageFilterBase::tr("MessageFilterBase is an abstract class"));
        qRegisterMetaType<QMessageId>("QMessageId");
        qRegisterMetaType<QMessageIdList>("QMessageIdList");
    }
};

Q_EXPORT_PLUGIN2(qmlmessaging, QDeclarativeMessageModelPlugin);

#include "plugin.moc"

