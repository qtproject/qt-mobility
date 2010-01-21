#include <QtCore>
#include <QtGui>
#include <QtDeclarative>
#include <qaccelerometer.h>
#include <qorientationsensor.h>

QTM_USE_NAMESPACE

QML_DECLARE_TYPE(QOrientationSensor);
QML_DEFINE_TYPE(Qt, 4, 6, OrientationSensor, QOrientationSensor);

QML_DECLARE_TYPE(QSensorReading);
QML_DEFINE_NOCREATE_TYPE(QSensorReading);

QML_DECLARE_TYPE(QOrientationReading);
//### while we don't want to explicitly create this type in QML, we need to define it so
//    we can specify enum values like OrientationReading.LeftUp
QML_DEFINE_TYPE(Qt, 4, 6, OrientationReading, QOrientationReading);

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget();

private:
    QmlView *view;
};

MainWidget::MainWidget()
{
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    view = new QmlView(this);
    vbox->addWidget(view);

    view->setUrl(QUrl("qrc:/orientation.qml"));
    view->execute();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWidget mainWidget;
    mainWidget.show();

    return app.exec();
}

#include "main.moc"

