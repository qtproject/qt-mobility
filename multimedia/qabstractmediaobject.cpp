class QAbstactMediaObject : public QObject
{
public:
    virtual void setUserObject(QObject* userObject);

protected:
    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) = 0;

    bool event(QEvent* e)
    {
        if (e->type() == QEvent::DynamicPropertyChange) {
            QByteArray pn = qobject_cast<QDynamicpropertyChangeEvent*>(e)->propertyName();
            QVariant pv = property(pn.constData());
            if (!propertyValueChanged(pn, pv))
                return false;

            const QMetaObject* m = userObject->metaObject();

            int pi = m->indexOfProperty(pn);
            if (pi == -1)
                return false;

            QMetaProperty p = m->property(pi);
            if (!p.hasNotifySignal()) {
                return false

            p->notifySignal().invoke(userObject, QGenericArgument(QMetaType::typeName(p.userType), property(pn).data()));

            e->accept();
            return true;
        }

        return false;
    }

    QObject* userObject;
};


