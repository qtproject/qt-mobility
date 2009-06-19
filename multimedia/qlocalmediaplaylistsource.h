#ifndef QLOCALMEDIAPAYLISTSOURCE_H
#define QLOCALMEDIAPAYLISTSOURCE_H

#include "qmediaplaylistsource.h"

class QLocalMediaPlaylistSourcePrivate;
class QLocalMediaPlaylistSource : public QMediaPlaylistSource
{
    Q_OBJECT
public:
    QLocalMediaPlaylistSource(QObject *parent=0);
    virtual ~QLocalMediaPlaylistSource();

    virtual int size() const;
    virtual QMediaSource itemAt(int pos) const;

    virtual bool isReadOnly() const;

    virtual bool append(const QMediaSource &source);
    virtual bool append(const QList<QMediaSource> &sources);
    virtual bool insert(int pos, const QMediaSource &source);
    virtual bool remove(int pos);
    virtual bool remove(int start, int end);
    virtual bool clear();

private:
    Q_DECLARE_PRIVATE(QLocalMediaPlaylistSource)
};

#endif // QLOCALMEDIAPAYLISTSOURCE_H
