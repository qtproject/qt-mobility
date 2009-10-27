
#ifndef QCRMLPARSER_H
#define QCRMLPARSER_H

#include <QList>
#include <QString>
#include <QXmlStreamReader>

class KeyData{
public:
    KeyData():m_UID(0), m_bitIndex(0){}
    KeyData(const QString &path, quint64 UID, quint32 bitmask=0);
    quint64 uid() const {return m_UID;}
    quint32 bitIndex() const { return m_bitIndex;}
    QString path()const {return m_path;}

private:
    quint64 m_UID;
    QString m_path;
    quint32 m_bitIndex;
};

class QCrmlParser : private QXmlStreamReader
{
public:
    enum Error{NoError, FileNotFound, FileOpenError, ParseError};
    QList<KeyData> parseQcrml(const QString &filePath);
    Error error();
    QString errorString();
private:
    void parseUnknownElement();
    QList<KeyData> parseRepository();
    QList<KeyData> parseKey(quint32 repoUid);
    QList<KeyData> parseKeyRange(quint32 repoUid);
    QList<KeyData> parseBit(quint32 repoUid, quint32 keyInt);

    bool checkMandatoryAttributes(const QStringList &mandatoryAttributes);
    quint32 uidStringToUInt32(const QString &uidString, bool*ok);
    void setError(Error error, const QString &errorString);

    Error m_error;
    QString m_errorString;
};

#endif


