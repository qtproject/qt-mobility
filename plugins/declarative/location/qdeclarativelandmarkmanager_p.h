#ifndef QDECLARATIVELANDMARKMANAGER_P_H
#define QDECLARATIVELANDMARKMANAGER_P_H

#include <qlandmark.h>
#include <qlandmarkmanager.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkManager : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
    explicit QDeclarativeLandmarkManager(QObject* parent = 0);
    ~QDeclarativeLandmarkManager();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QString name();


signals:

public slots:

private:

    QLandmarkManager* m_manager;

};


QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkManager));


#endif // QDECLARATIVELANDMARKMANAGER_P_H
