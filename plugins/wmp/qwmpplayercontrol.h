#ifndef QWMPPLAYERCONTROL_H
#define QWMPPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include "qwmpglobal.h"

#include <wmp.h>

class QUrl;

class Q_WMP_EXPORT QMediaPlayerControl : public QObject
{
    Q_OBJECT
public:
    QMediaPlayerControl(QObject *parent = 0) : QObject(parent) {}

    virtual QUrl url() const = 0;
    virtual void setUrl(const QUrl &url) = 0;

public Q_SLOTS:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

class Q_WMP_EXPORT QWmpPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
public:
    QWmpPlayerControl(IWMPCore3 *player, QObject *parent = 0);
    ~QWmpPlayerControl();


    QUrl url() const;
    void setUrl(const QUrl &url);

public Q_SLOTS:
    void play();
    void pause();
    void stop();

private:
    IWMPCore3 *m_player;
    IWMPControls *m_controls;
    IWMPSettings *m_settings;
};

#endif
