#ifndef QMEDIAPLAYLISTSOURCE_H
#define QMEDIAPLAYLISTSOURCE_H

#include <QObject>
#include "qmediasource.h"
#include "qmediaplaylist.h"

class QString;

class QMediaPlaylistSourcePrivate;
class QMediaPlaylistSource : public QObject
{
Q_OBJECT
public:
    QMediaPlaylistSource(QObject *parent=0);
    virtual ~QMediaPlaylistSource();

    virtual bool load(const QString &location, const char *format = 0);
    virtual bool load(QIODevice * device, const char *format = 0);
    virtual bool save(const QString &location, const char *format = 0);
    virtual bool save(QIODevice * device, const char *format);

    virtual int size() const = 0;
    virtual QMediaSource itemAt(int pos) const = 0;

    virtual bool isReadOnly() const;

    virtual bool append(const QMediaSource &source);
    virtual bool append(const QList<QMediaSource> &sources);
    virtual bool insert(int pos, const QMediaSource &source);
    virtual bool remove(int pos);
    virtual bool remove(int start, int end);
    virtual bool clear();

public Q_SLOTS:
    virtual void shuffle();

Q_SIGNALS:
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted();
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved();
    void itemsChanged(int start, int end);

protected:
    QMediaPlaylistSource(QMediaPlaylistSourcePrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylistSource);
};

#endif // QMEDIAPLAYLISTSOURCE_H
