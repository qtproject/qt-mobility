
#include <QtCore/qcoreapplication.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qdir.h>

#include "qmediaserviceproviderplugin.h"

#include "qmediapluginloader_p.h"


QMediaPluginLoader::QMediaPluginLoader(const char *iid, const QString &location, Qt::CaseSensitivity):
    m_iid(iid)
{
    m_location = location + "/";
    load();
}

QStringList QMediaPluginLoader::keys() const
{
    return m_keys.toList();
}

QObject* QMediaPluginLoader::instance(QString const &key)
{
    foreach (QMediaServiceProviderPlugin* plugin, m_providers) {
        if (plugin->keys().contains(key))
            return plugin;
    }

    return 0;
}

void QMediaPluginLoader::load()
{
    if (!m_providers.isEmpty())
        return;

    QStringList     paths = QCoreApplication::libraryPaths();

    foreach (QString const &path, paths) {
        QString     pluginPathName(path + m_location);
        QDir        pluginDir(pluginPathName);

        if (!pluginDir.exists())
            continue;

        foreach (QString pluginLib, pluginDir.entryList(QDir::Files)) {
            QPluginLoader   loader(pluginPathName + pluginLib);

            QObject *o = loader.instance();
            if (o != 0 && o->qt_metacast(m_iid) != 0) {
                QMediaServiceProviderPlugin* p = qobject_cast<QMediaServiceProviderPlugin*>(o);
                if (p != 0) {
                    foreach (QString const &key, p->keys())
                        m_keys << key;

                    m_providers << p;
                }

                continue;
            }

            delete o;
            loader.unload();
        }
    }
}
