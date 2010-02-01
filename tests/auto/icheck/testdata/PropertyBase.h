#include <QtCore>
#include <QString>
#include <QObject>

class IPropertyBaseImpl : public QObject
{
    Q_OBJECT
public:
    //assign flags to metadata
    Q_FLAGS(MyFlags)
    enum enumFlag {
        enumFlag0 = 0,
        enumFlag1,
        enumFlag2,
        enumFlag3
    };
    //define a enum as a flag
    Q_DECLARE_FLAGS(MyFlags, enumFlag)
    Q_PROPERTY( MyFlags Flag READ readFlag NOTIFY notifyFlag);

    MyFlags readFlag() const
    {
        return enumFlag0;
    }

    void notifyFlag();
};
