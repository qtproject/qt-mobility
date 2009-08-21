
#ifndef QMEDIAPLUGINLOADER_H
#define QMEDIAPLUGINLOADER_H

#include <QtCore/qstring.h>
#include <QtCore/qset.h>

class QMediaServiceProviderPlugin;


class QMediaPluginLoader
{
public:
    QMediaPluginLoader(const char *iid,
                   const QString &suffix = QString(),
                   Qt::CaseSensitivity = Qt::CaseSensitive);

    QStringList keys() const;
    QObject* instance(QString const &key);

private:
    void load();

    QByteArray  m_iid;
    QString     m_location;
    QSet<QString> m_keys;
    QList<QMediaServiceProviderPlugin*> m_providers;
};

#endif  // QMEDIAPLUGINLOADER_H
