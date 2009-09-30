class QVersitProperty
{
public:
    QVersitProperty() {}
    ~QVersitProperty() {}

    // might not be required!
    QVersitDocument::VersitType versitType() const = 0;

    enum Error {
        InvalidKey = 0,
        InvalidTypes,
        InvalidValueType,
        IncompatibleData,
        IncompatibleDetail,
        InvalidEncoding,
        InvalidFormat
    };

    virtual Error error() const = 0;

    enum BinaryEncoding {
        // whatever values required here.
        Base64 = 0,
        Uucode,
    };

    enum CharacterEncoding {
        // whatever values required here.
        Latin1 = 0,
        Utf8,
        Utf16,
        Utf32
    };

    enum ImageFormat {
        // whatever values required here.
        Jpeg = 0,
        Bitmap,
        Gif,
        Png,
        Svg
    };

    virtual QString key() const = 0;
    virtual QStringList types() const = 0;
    virtual QPair<QVariant, Encoding> value() const = 0;
    virtual QByteArray value() const = 0;

    virtual void setKey(const QString& key) = 0;         // TEL,ADR,FN,etc
    virtual void setTypes(const QStringList& types) = 0; // TYPE=home,work,pref,voice,cell,intl,dom,post,etc
    virtual void setValue(const QString& value, CharacterEncoding encoding) = 0;
    virtual void setValue(const QByteArray& value, BinaryEncoding encoding) = 0;
    virtual void setValue(const QImage& value, ImageFormat format) = 0;

    // note that some properties can have an embedded versit property (or versit document) (eg AGENT)
    virtual void setValue(const QVersitDocument& embedded) = 0;

    virtual QStringList validKeys() const = 0;
    virtual QStringList validTypes(const QString& key = QString()) const = 0;
    virtual QVariant::Type valueType() const = 0;
};

class QVersitDocument
{
public:
    QVersitDocument() {}
    ~QVersitDocument() {}

    enum VersitType {
        VCard21 = 0,
        VCard30,
        VCardDav,
        VCalendar10,
        ICalendar20
        // ... etc.
    };

    // metadata about the versit document itself.
    virtual void setVersitType(VersitType type) = 0;
    virtual void setPublisher(const QString& publisher) = 0;
    virtual void setProfile(const QString& profile) = 0;
    virtual void setSource(const QString& source) = 0;
    virtual VersitType versitType() const = 0;
    virtual QString publisher() const = 0;
    virtual QString profile() const = 0;
    virtual QString source() const = 0;
    // etc.

    virtual QList<QVersitProperty> properties() const = 0;
    virtual void setProperties(QList<QVersitProperty>& properties) const = 0;
};

// reads QVersitDocument from iodevice
class QVersitReader : public QObject
{
    Q_OBJECT

public:
    QVersitReader() : QObject() {}
    ~QVersitReader() {}

    // async read / write operation
    virtual bool setDevice(QIODevice* device) = 0;
    virtual QIODevice* device() const = 0;
    virtual bool start() = 0;

    // output:
    virtual QVersitDocument result() const = 0;

signals:
    void progress(bool finished);
};

// writes QVersitDocument to iodevice
class QVersitWriter : public QObject
{
    Q_OBJECT

public:
    QVersitWriter() : QObject() {}
    ~QVersitWriter() {}

    // input:
    virtual bool setVersitDocument(const QVersitDocument& versitDocument) = 0;
    virtual QVersitDocument versitDocument() const = 0;

    // async read / write operations.
    virtual bool setDevice(QIODevice* device) = 0;
    virtual QIODevice* device() const = 0;
    virtual bool start() = 0;

signals:
    void progress(bool finished);
};

// Generates a list of contacts from a QVersitDocument
class QVersitContactGenerator
{
public:
    QVersitContactGenerator() {}
    ~QVersitContactGenerator() {}

    enum Error {
        // ...
    };

    Error error() const;

    QList<QContact> generateContacts(const QVersitDocument& versitDocument);
};

// Converts a list of contacts to a QVersitDocument
class QVersitContactConverter
{
public:
    QVersitContactConverter() {}
    ~QVersitContactConverter() {}

    enum VersitType {
        VCard21 = 0,
        VCard30,
        VCardDav,
        VCalendar10,
        ICalendar20
        // ... etc.
    };

    enum BinaryEncoding {
        // whatever values required here.
        Base64 = 0,
        Uucode,
    };

    enum CharacterEncoding {
        // whatever values required here.
        Latin1 = 0,
        Utf8,
        Utf16,
        Utf32
    };

    enum ImageFormat {
        // whatever values required here.
        Jpeg = 0,
        Bitmap,
        Gif,
        Png,
        Svg
    };

    // knobs that affect behaviour of the converter
    void setVersitType(VersitType type);
    void setBinaryEnconding(BinaryEncoding encoding);
    void setCharacterEncoding(CharacterEncoding encoding);
    void setImageFormat(ImageFormat format);

    // accessors for those knobs
    VersitType versitType() const;
    BinaryEncoding binaryEncoding() const;
    CharacterEncoding characterEncoding() const;
    ImageFormat imageFormat() const;

    // blob provider (eg, image detail value -> QPixmap in a QVariant)
    virtual QVariant loadBlob(const QContactDetail& detail, const QString& fieldName);

    enum Error {
        // ...
    };

    Error error() const;

    // convert the list of contacts into a versit document.
    QVersitDocument convertContacts(const QList<QContact>& contacts);
};
