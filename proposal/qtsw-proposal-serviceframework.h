/*
 * This document contains a proposal for interface definitions which
 * provide integration with the Qt Service Framework (QSFW) module
 * of the Mobility API.
 *
 * QContactAbstractAction must be implemented by any plugin wishing
 * to provide clients the ability to perform actions on details of a
 * QContact.
 *
 * The provider must also implement QServicePluginInterface in accordance
 * with the Qt Service Framework documentation.
 */

class QContactAbstractAction : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactAbstractAction();
    virtual QStringList actions() const;
    virtual QStringList actionsForContact(const QContact& contac) const;
    virtual QStringList actionsForDetail(const QContactDetail& detail) const;
    virtual bool supportsAction(const QString& actionId);
    virtual bool supportsDetail(const QString& actionId, const QContactDetail& detail) const;
    virtual QList<QContactDetail> supportedDetails(const QString& actionId, const QContact& contact) const;
    virtual QVariantMap metadata(const QString& actionId) const;
    virtual void performAction(const QString& actionId, const QContact& contact, const QContactDetail& detail = QContactDetail()) = 0;

    QServiceInterfaceDescriptor descriptor(); // needed?

    static QList<QContactAbstractAction*> implementations(const QString& actionId) const;

private:
    QSharedDataPointer<QContactAbstractActionData> d;
};