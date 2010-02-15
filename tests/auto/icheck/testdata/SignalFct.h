#include "SignalBase.h"

class IMyInterfaceImpl : public IMyInterfaceBaseImpl
{
public:
    //successful part
    Q_SIGNAL void signalFct1();
    Q_SIGNAL void signalFct2();
    Q_SIGNAL void signalFct3(long arg1, double arg2, QObject arg3);
    Q_SIGNAL void signalFct4(bool arg1, QString arg2, int arg3);

    Q_SIGNAL void signalFct5(const long& arg1, const double& arg2, const QObject& arg3);
    Q_SIGNAL void signalFct6(const bool& arg1, const QString& arg2, const int& arg3);
    Q_SIGNAL void signalFct7(const long* arg1, const double* arg2, const QObject* arg3);
    Q_SIGNAL void signalFct8(const bool* arg1, const QString* arg2, const int* arg3);

signals:
    void signalFct9(long& arg1, double& arg2, QObject& arg3);
    void signalFct10(bool& arg1, QString& arg2, int& arg3);
    void signalFct11(long* arg1, double* arg2, QObject* arg3);
    void signalFct12(bool* arg1, QString* arg2, int* arg3);
Q_SIGNALS:
    void signalFct13(long arg1, double arg2, QObject arg3);
    void signalFct14(bool arg1, QString arg2, int arg3);
signals:
    void signalFct15(long arg1, double arg2, QObject arg3);
    void signalFct16(bool arg1, QString arg2, int arg3);

    //unsuccessful  part
    Q_SIGNAL QString signalFct17(bool arg1, QString arg2, int arg3); //wrong return type, must be void
    Q_SIGNAL void differentnames1(bool arg1, QString arg2, int arg3); //wrong name
    Q_SIGNAL void overloadedDiffFct(int arg1, QString arg2, int arg3);//first pparameter should be bool

slots:
    void noSingnal();//slot is wrong, must be signal
};
