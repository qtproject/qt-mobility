#ifndef QDECLARATIVEFILEEFFECT_H
#define QDECLARATIVEFILEEFFECT_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>

QTM_USE_NAMESPACE

class QDeclarativeFileEffect : public QFeedbackFileEffect
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
public:
    QDeclarativeFileEffect(QObject *parent = 0) : QFeedbackFileEffect(parent), running_(false), paused_(false) {
        QObject::connect(this, SIGNAL(stateChanged()), this, SLOT(updateState()));
    }

    bool isRunning() const { return running_; }
    bool isPaused() const { return paused_; }
    void setRunning(bool running) {
        State currentState = state();
        if (currentState != Running && running) {
            start();
        } else if (currentState != Stopped && !running) {
            stop();
        }
    }
    void setPaused(bool paused) {
        State currentState = state();
        if (currentState == Paused && !paused) {
            start();
        } else if (currentState == Running && paused) {
            pause();
        }
    }

signals:
    void runningChanged();
    void pausedChanged();
public slots:
    void updateState() {
        bool running = state() == Running;
        bool paused = state() == Paused;
        if (running != running_) {
            running_ = running;
            emit runningChanged();
        }
        if (paused != paused_) {
            paused_ = paused;
            emit pausedChanged();
        }
    }
private:
    bool running_;
    bool paused_;
};

QML_DECLARE_TYPE(QDeclarativeFileEffect);

#endif // QDECLARATIVEFILEEFFECT_H
