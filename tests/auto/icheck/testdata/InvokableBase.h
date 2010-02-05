#include <QtCore>
#include <QString>
#include <QObject>

class IMyInterfaceBaseImpl : public QObject
{
    Q_OBJECT
public:
    //successful part
    Q_INVOKABLE QString implementedInBase(bool arg1, QString arg2, int arg3);
};
