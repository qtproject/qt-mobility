

#include <QMediaPlayerService>


class MpdDaemon;
class MpdPlayerControl;

class MpdPlayerService : public QMediaPlayerService
{
    Q_OBJECT

public:
    MpdPlayerService();
    ~MpdPlayerService();

    virtual MpdPlayerControl* control() const;

private slots:
    void stateChanged(int state);
    void disconnected();

private:
    MpdDaemon *daemon;
    MpdPlayerControl *control;
};
