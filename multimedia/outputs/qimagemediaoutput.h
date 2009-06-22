#ifndef QIMAGEMEDIAOUTPUT_H
#define QIMAGEMEDIAOUTPUT_H

#include <QtCore/qplugin.h>
#include <QtCore/qsize.h>

class QImageMediaOutputInterface
{
public:
    virtual ~QImageMediaOutputInterface();
};

#define QImageMediaOutputInterface_iid "com.nokia.Qt.QImageMediaOutputInterface/1.0"

Q_DECLARE_INTERFACE(QImageMediaOutputInterface, QImageMediaOutputInterface_iid)

class QImage;

class QImageMediaOutputPrivate;

class QImageMediaOutput : public QObject, public QImageMediaOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(QImageMediaOutputInterface)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool hasImage READ hasImage)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_DECLARE_PRIVATE(QImageMediaOutput)
public:
    enum State
    {
        NoImage,
        PartialImage,
        CompleteImage
    };

    QImageMediaOutput(QObject *parent = 0);
    ~QImageMediaOutput();

    QImage image() const;
    State state() const;

    bool hasImage() const;

    QSize maximumSize() const;
    virtual void setMaximumSize(const QSize &size);

Q_SIGNALS:
    void imageChanged(const QImage &image);
    void stateChanged(QImageMediaOutput::State state);

protected:
    void setImage(const QImage &image, State state);
};

#endif
