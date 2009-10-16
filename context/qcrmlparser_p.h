
#ifndef QCRMLPARSER_H
#define QCRMLPARSER_H

#include <QList>
#include <QString>
class QDomElement;

class KeyData{
public:
    KeyData():m_UID(0), m_bitIndex(0){}
    KeyData(const QString &path, quint64 UID, quint32 bitmask=0);
    quint64 UID() const {return m_UID;}
    quint32 bitIndex() const { return m_bitIndex;}
    QString path()const {return m_path;}

private:
    quint64 m_UID;
    quint32 m_bitIndex;
    QString m_path;
};

class QCRMLParser {
public:
    enum ErrorType{NoError, FileDoesNotExist, ParseError};
    QList<KeyData> parseQCRML(const QString &filePath);
    ErrorType error();
    QString errorString();
private:
    void setError(ErrorType type, const QString &errorString);
    QList<KeyData> parseKey(const QDomElement &element);
    bool parseRepository(const QDomElement &element);
    QList<KeyData> parseKeyRange(const QDomElement &element);
    uint uidStringToUInt32(const QString &uid, bool *ok=false);
    bool checkMandatoryAttributes(const QDomElement &element,
                                  const QStringList &mandatoryAttributes);

    ErrorType m_error;
    QString m_errorString;

    quint32 m_repoUID;
    QList<KeyData> m_keyData;
};
#endif
