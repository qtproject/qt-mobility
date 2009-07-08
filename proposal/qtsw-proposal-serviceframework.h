/*
 * This document contains a proposal for interface definitions which
 * provide integration with the Qt Service Framework (QSFW) module
 * of the Mobility API.
 *
 * QContactActionFactory must be implemented by any plugin wishing
 * to provide clients the ability to perform actions on details of a
 * QContact.
 */

class QContactActionFactory : public QObject
{
    Q_OBJECT
    Q_INTERFACE(...)

public:
    QStringList actions() const;
    QString id() const;
    int majorVersion() const; // perhaps
    int minorVersion() const;

    QContactAbstractAction* instance(const QString& action) const;
};

class QContactAbstractAction : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactAbstractAction();
    virtual QString actionId() const;

    virtual QVariantMap metadata() const; // label, icon etc

    virtual QContactFilter contactFilter() const; // use for matching

    virtual bool supportsDetail(const QContactDetail& detail) const;
    virtual QList<QContactDetail> supportedDetails(const QContact& contact) const;

    virtual void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail()) = 0;

    /* This goes in QCM, probably  */
    static QList<QContactAbstractAction*> supportedActions(const QContactActionFilter& filter) const;
private:
    QSharedDataPointer<QContactAbstractActionData> d;
};
