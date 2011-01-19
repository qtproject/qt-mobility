#include "program_options.h"

#include <QApplication>
#include <QStringList>

static QVariantHash get_program_options()
{
    QVariantHash parameters;

    QStringList args = QApplication::arguments();

    while (!args.isEmpty()) {
        QString word = args.takeFirst();
        if (word[0] == QChar('-')) {
            word.remove(0, 1);
            if (args.isEmpty() || args.first()[0] == QChar('-')) {
                parameters[word] = true;
            }
            else {
                QString value = args.takeFirst();
                if (value == "true" || value == "on" || value == "enabled") {
                    parameters[word] = true;
                }
                else if (value == "false" || value == "off" || value == "disabled") {
                    parameters[word] = false;
                }
                else {
                    bool ok = false;
                    double realValue = value.toDouble(&ok);
                    if (ok) {
                        parameters[word] = realValue;
                    }
                    else {
                        parameters[word] = value;
                    }
                }
            }
        }
    }

    return parameters;
}

const QVariantHash & program_options()
{
    static QVariantHash program_options_ = get_program_options();
    return program_options_;
}
