#ifndef QMEDIAIMAGEVIEWER_H
#define QMEDIAIMAGEVIEWER_H

#include <QWidget>


class QMediaimageViewerPrivate;

class QMediaImageViewer : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QMediaSource mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(int interval READ interval NOTIFY intervalChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_ENUMS(State)

public:
    enum State { LoadingState, PlayingState, PausedState, StoppedState };

    // Should also have session so can grab custom uri schemes etc?
    QMediaImageViewer(QWidget* parent = 0);
    ~QMediImageViewer();

    int interval() const;
    State state() const;

public Q_SLOTS:
    void setMediaSource(QMediaSource mediaSource);

    void play();
    void pause();
    void stop();

    void skipForward();
    void skipBackward();

Q_SIGNALS:
    void intervalChanged(int interval);
    void stateChanged(QMediaImageViewer::State state);

private:
    Q_DECLARE_PRIVATE(QMediaImageViewer)
    Q_DISABLE_COPY(QMediaImageViewer)
};

#endif // QMEDIAIMAGEVIEWER_H
