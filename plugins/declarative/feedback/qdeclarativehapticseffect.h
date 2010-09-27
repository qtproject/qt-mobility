#ifndef QDECLARATIVEHAPTICSEFFECT_H
#define QDECLARATIVEHAPTICSEFFECT_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>

QTM_USE_NAMESPACE

class QDeclarativeHapticsEffect : public QFeedbackHapticsEffect
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QFeedbackActuator> actuators READ actuators)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool supportsThemeEffect READ themeEffectSupported)

public:
    QDeclarativeHapticsEffect(QObject *parent = 0) : QFeedbackHapticsEffect(parent), running_(false), paused_(false) {
        actuators_ = QFeedbackActuator::actuators();
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

    bool themeEffectSupported() {
        return supportsThemeEffect();
    }
    Q_INVOKABLE void playEffect(ThemeEffect effect) {
        playThemeEffect(effect);
    }

    QDeclarativeListProperty<QFeedbackActuator> actuators() {
        return QDeclarativeListProperty<QFeedbackActuator>(this,
                0,
                actuator_append,
                actuator_count,
                actuator_at,
                0);
    }
    static void actuator_append(QDeclarativeListProperty<QFeedbackActuator> *prop, QFeedbackActuator *actuator)
    {
        static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.append(actuator);
    }
    static int actuator_count(QDeclarativeListProperty<QFeedbackActuator> *prop)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.size();
    }
    static QFeedbackActuator *actuator_at(QDeclarativeListProperty<QFeedbackActuator> *prop, int index)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.at(index);
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
public:
    bool running_;
    bool paused_;
    QList<QFeedbackActuator*> actuators_;
};

QML_DECLARE_TYPE(QDeclarativeHapticsEffect);

#endif // QDECLARATIVEHAPTICSEFFECT_H
