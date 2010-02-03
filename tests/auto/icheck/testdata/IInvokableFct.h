class IMyInterface 
{
public:
    //successful part
    Q_INVOKABLE void invokeFct1();
    Q_INVOKABLE bool invokeFct2();
    Q_INVOKABLE QString invokeFct3(long arg1, double arg2, QObject arg3);
    Q_INVOKABLE QString invokeFct4(bool arg1, QString arg2, int arg3);

    Q_INVOKABLE QString invokeFct5(const long& arg1, const double& arg2, const QObject& arg3);
    Q_INVOKABLE QString invokeFct6(const bool& arg1, const QString& arg2, const int& arg3);
    Q_INVOKABLE QString invokeFct7(const long* arg1, const double* arg2, const QObject* arg3);
    Q_INVOKABLE QString invokeFct8(const bool* arg1, const QString* arg2, const int* arg3);

    Q_INVOKABLE QString invokeFct9(long& arg1, double& arg2, QObject& arg3);
    Q_INVOKABLE QString invokeFct10(bool& arg1, QString& arg2, int& arg3);
    Q_INVOKABLE QString invokeFct11(long* arg1, double* arg2, QObject* arg3);
    Q_INVOKABLE QString invokeFct12(bool* arg1, QString* arg2, int* arg3);

    Q_INVOKABLE QString invokeFct13(long arg1, double arg2, QObject arg3);
    Q_INVOKABLE QString invokeFct14(bool arg1, QString arg2, int arg3);
    Q_INVOKABLE QString invokeFct15(long arg1, double arg2, QObject arg3);
    Q_INVOKABLE QString invokeFct16(bool arg1, QString arg2, int arg3);

    //this is implemented in the base class
    Q_INVOKABLE QString implementedInBase(bool arg1, QString arg2, int arg3);

    //unsuccessful  part
    Q_INVOKABLE QString invokeFct17(bool arg1, QString arg2, int arg3); //missing
    Q_INVOKABLE QString diffname1(bool arg1, QString arg2, int arg3); //different name
    Q_INVOKABLE void overloadedDiffFct(bool arg1, QString arg2, int arg3);//argument list is different
    Q_INVOKABLE void notInvokable();

};
