#include "explorer.h"
#include <QTimer>
#include <QDebug>
#include <qsensor.h>
#include <QMetaObject>
#include <QMetaProperty>

QTM_USE_NAMESPACE

Explorer::Explorer(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    clearReading();
    m_reading->installEventFilter(this);
}

Explorer::~Explorer()
{
}

void Explorer::loadSensors()
{
    qDebug() << "Explorer::loadSensors";

    m_sensors->clear();

    foreach (const QByteArray &type, QSensor::sensorTypes()) {
        qDebug() << "Found type" << type;
        QTreeWidgetItem *typeItem = new QTreeWidgetItem(QStringList() << QString::fromLatin1(type));
        bool added = false;
        foreach (const QByteArray &identifier, QSensor::sensorsForType(type)) {
            qDebug() << "Found identifier" << identifier;
            // Don't put in sensors we can't connect to
            QSensor sensor;
            sensor.setType(type);
            sensor.setIdentifier(identifier);
            if (!sensor.connect()) {
                qDebug() << "Couldn't connect to" << identifier;
                continue;
            }

            if (!added) {
                qDebug() << "Adding type" << type;
                m_sensors->addTopLevelItem(typeItem);
                added = true;
            }
            qDebug() << "Adding identifier" << identifier;
            (void)new QTreeWidgetItem(typeItem, QStringList() << QString::fromLatin1(identifier));
        }
        if (!added) {
            qDebug() << "Didn't add type" << type;
            delete typeItem;
        }
    }

    m_sensors->expandAll();
}

void Explorer::clearReading()
{
    m_reading->setRowCount(0);
}

void Explorer::loadReading()
{
    qDebug() << "Explorer::loadReading";
    clearReading();

    QTreeWidgetItem *item = m_sensors->currentItem();
    if (!item) {
        qWarning() << "Didn't select an item!";
        return;
    }

    QTreeWidgetItem *parent = item->parent();
    if (!parent) {
        qWarning() << "Didn't select a sensor!";
        return;
    }

    QByteArray type = parent->data(0, Qt::DisplayRole).toString().toLatin1();
    QByteArray identifier = item->data(0, Qt::DisplayRole).toString().toLatin1();
    QSensor sensor;
    sensor.setType(type);
    sensor.setIdentifier(identifier);
    if (!sensor.connect()) {
        qWarning() << "Can't connect to the sensor!";
        return;
    }

    QSensorReading *reading = sensor.reading();
    const QMetaObject *metaObject = reading->metaObject();
    int firstProperty; //= metaObject->propertyOffset();
    // Get properties from QSensorReading too (ie. timestamp)
    firstProperty = QSensorReading::staticMetaObject.propertyOffset();

    m_reading->setRowCount(metaObject->propertyCount() - firstProperty);

    for(int i = firstProperty; i < metaObject->propertyCount(); ++i) {
        QTableWidgetItem *prop = new QTableWidgetItem(metaObject->property(i).name());
        QTableWidgetItem *type = new QTableWidgetItem(metaObject->property(i).typeName());
        int row = i - firstProperty;
        m_reading->setItem(row, 0, prop);
        m_reading->setItem(row, 1, type);
    }

    adjustReadingColumns();
}

void Explorer::showEvent(QShowEvent *event)
{
    QTimer::singleShot(0, this, SLOT(loadSensors()));
    QMainWindow::showEvent(event);
}

void Explorer::adjustReadingColumns()
{
    m_reading->resizeColumnsToContents();

    int propWidth = m_reading->columnWidth(0);
    int typeWidth = m_reading->columnWidth(1);

    int suggestedWidth = propWidth + typeWidth;
    int actualWidth = m_reading->size().width();
    //qDebug() << "suggestedWidth" << suggestedWidth << "actualWidth" << actualWidth;

    if (actualWidth > suggestedWidth) {
        qreal multiplier = actualWidth / static_cast<qreal>(suggestedWidth);
        propWidth = multiplier * propWidth;
        typeWidth = multiplier * typeWidth;

        // FIXME this is probably some frame width on each side of the cell
        propWidth -= 2;
        typeWidth -= 2;

        m_reading->setColumnWidth(0, propWidth);
        m_reading->setColumnWidth(1, typeWidth);
    }
}

bool Explorer::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_reading && event->type() == QEvent::Resize) {
        adjustReadingColumns();
    }

    return QMainWindow::eventFilter(obj, event);
}

