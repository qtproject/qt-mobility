#ifndef QIMAGEMODELMEDIAOUTPUT_H
#define QIMAGEMODELMEDIAOUTPUT_H

#include <QtCore/qplugin.h>
#include <QtCore/qsize.h>

class QImageModelMediaOutputInterface
{
public:
    virtual ~QImageModelMediaOutputInterface();
};

#define QImageModelMediaOutputInterface_iid "com.nokia.Qt.QImageModelMediaOutputInterface/1.0"

Q_DECLARE_INTERFACE(QImageModelMediaOutputInterface, QImageModelMediaOutputInterface_iid)

class QImage;

class QImageModelMediaOutputPrivate;

class QImageModelMediaOutput : public QObject, public QImageModelMediaOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(QImageModelMediaOutputInterface)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QSize maximumImageSize READ maximumImageSize WRITE setMaximumImageSize)
    Q_PROPERTY(int maximumImagesLoaded READ maximumImagesLoaded WRITE setMaximumImagesLoaded)
    Q_DECLARE_PRIVATE(QImageModelMediaOutput)
public:
    enum ImageState
    {
        NoImage,
        LoadingImage,
        PartialImage,
        CompleteImage
    };

    QImageModelMediaOutput(QObject *parent = 0);
    ~QImageModelMediaOutput();

    int count() const;

    int currentIndex() const;

    virtual QImage image(int index) const = 0;
    virtual ImageState imageState(int index) const = 0;

    int maximumImagesLoaded() const;
    virtual void setMaximumImagesLoaded(int maximum);

    QSize maximumImageSize() const;
    virtual void setMaximumImageSize(const QSize &size);

Q_SIGNALS:
    void imageChanged(int index, const QImage &image);
    void imageStateChanged(int index, QImageModelMediaOutput::ImageState state);

    void currentIndexChanged(int index);

    void countChanged(int count);
    void imagesInserted(int index, int count);
    void imagesRemoved(int index, int count);

protected:
    void setCurrentIndex(int index);
    void insertImages(int index, int count);
    void removeImages(int index, int count);
};

#endif
