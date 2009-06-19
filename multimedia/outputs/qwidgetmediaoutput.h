#ifndef QWIDGETMEDIAOUTPUT_H
#define QWIDGETMEDIAOUTPUT_H

#include <QtCore/qplugin.h>
#include <QtGui/qwidget.h>

class QWidgetMediaOutputInterface
{
public:
    virtual ~QWidgetMediaOutputInterface();
};

#define QWidgetMediaOutputInterface_iid "com.nokia.Qt.QWidgetMediaOutputInterface/1.0"

Q_DECLARE_INTERFACE(QWidgetMediaOutputInterface, QWidgetMediaOutputInterface_iid)

class QWidgetMediaOutputPrivate;

class QWidgetMediaOutput : public QWidget, public QWidgetMediaOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(QWidgetMediaOutputInterface)
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
    Q_DECLARE_PRIVATE(QWidgetMediaOutput)
public:
    QWidgetMediaOutput(QWidget *parent = 0);
    ~QWidgetMediaOutput();

    bool isFullscreen() const;

public Q_SLOTS:
    virtual void setFullscreen(bool fullscreen);

protected:
    QWidgetMediaOutput(QWidgetMediaOutputPrivate &dd, QWidget *parent);
};

#endif
