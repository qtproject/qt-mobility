#include <QMainWindow>
#include <ui_explorer.h>

class Explorer : public QMainWindow, public Ui::Explorer
{
    Q_OBJECT
public:
    Explorer(QWidget *parent = 0);
    ~Explorer();

private slots:
    void loadSensors();
    void loadReading();

private:
    void showEvent(QShowEvent *event);

    void clearReading();
    void adjustReadingColumns();
    bool eventFilter(QObject *obj, QEvent *event);
};

