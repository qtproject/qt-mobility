#ifndef QDECLARATIVELANDMARKCATEGORYMODEL_P_H
#define QDECLARATIVELANDMARKCATEGORYMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkmodel_p.h>
#include <qdeclarativelandmarkcategory_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmarkcategory.h>
#include <qlandmarkmanager.h>
#include <qlandmarkabstractrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkCategoryModel: public QDeclarativeLandmarkAbstractModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeLandmark* landmark READ landmark WRITE setLandmark NOTIFY landmarkChanged)

public:
    explicit QDeclarativeLandmarkCategoryModel(QObject* parent = 0);
    ~QDeclarativeLandmarkCategoryModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        NameRole = Qt::UserRole + 500, // Check
        DescriptionRole,
        IconSourceRole,
    };
    QDeclarativeLandmark* landmark() const;
    void setLandmark(QDeclarativeLandmark* landmark);
    int count() const;

signals:
    void countChanged();
    void landmarkChanged();

private slots:
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State state);

private:
    Q_INVOKABLE void startUpdate();
    void convertCategoriesToDeclarative();
    void setFetchRange();
    void setFetchFilters();

private:
    QLandmarkAbstractRequest* m_fetchRequest;
    // Landmark list received from platform
    QList<QLandmarkCategory> m_categories;
    // Same category list, but as declarative classes
    QMap<QString, QDeclarativeLandmarkCategory*> m_categoryMap;
    QDeclarativeLandmark* m_landmark;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategoryModel));

#endif // QDECLARATIVELANDMARKCATEGORYMODEL_P_H
