#ifndef QMEDIAPLAYLISTSOURCE_H
#define QMEDIAPLAYLISTSOURCE_H

#include <QObject>
#include "qmediasource.h"

class QMediaPlaylistSource : public QObject
{
Q_OBJECT
Q_PROPERTY(int currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
public:
    QMediaPlaylistSource();
    virtual ~QMediaPlaylistSource();

    virtual int size() const = 0;    
    virtual QMediaSource operator [](int pos) const = 0;

    virtual int currentItem();

    virtual bool isReadOnly() const;

    virtual bool append(const QMediaSource &source) = 0;
    virtual bool append(const QList<QMediaSource> &sources);
    virtual bool insert(int pos, const QMediaSource &source) = 0;
    virtual bool remove(int pos) = 0;
    virtual bool remove(int start, int end);
    virtual bool clear();

public Q_SLOTS:
    virtual void setCurrentItem(int pos);

Q_SIGNALS:
    void currentItemChanged(int);

    void itemsInserted(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);
};

#endif // QMEDIAPLAYLISTSOURCE_H

