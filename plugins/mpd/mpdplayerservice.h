

#include <QMediaPlayerService>


class MpdPlayerService : public QMediaPlayerService
{
    Q_OBJECT

public:
    MpdPlayerService();
    ~MpdPlayerService();

    virtual MpdPlayerService* control() const;

private slots:
    void stateChanged(int state);
    void disconnected();

private:
    MpdDaemon *daemon;
};
