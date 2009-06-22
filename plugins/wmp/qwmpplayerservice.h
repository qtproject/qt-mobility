#ifndef QWMPPLAYERSERVICE_H
#define QWMPPLAYERSERVICE_H

#include <QtCore/qobject.h>

#include "qwmpglobal.h"

#include <wmp.h>

class QMediaPlayerControl;

class Q_WMP_EXPORT QMediaPlayerService : public QObject
{
    Q_OBJECT
public:
    QMediaPlayerService(QObject *parent = 0) : QObject(parent) {}

    virtual QMediaPlayerControl *control() = 0;
};

class QWmpPlayerControl;

class Q_WMP_EXPORT QWmpPlayerService : public QMediaPlayerService
{
    Q_OBJECT
public:
    QWmpPlayerService(QObject *parent = 0);
    ~QWmpPlayerService();

    QMediaPlayerControl *control();

private:
    IWMPPlayer4 *m_player;
    QWmpPlayerControl *m_control;
};

#endif
