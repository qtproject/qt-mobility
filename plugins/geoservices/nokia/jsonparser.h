#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QVariant>
#include <QString>
#include <QRegExp>

class JSONParser
{
public:
    QString data;
    int pos;


    JSONParser(QString data) : data(data), pos(0) {}

    bool detectString(const QString & string) {
        skipWhitespace();
        return data.mid(pos, string.size()) == string;
    }

    void skipWhitespace() {
        while (pos < data.size() && data[pos].isSpace())
            ++pos;
    }

    char peek() const {
        return data[pos].toLatin1();
    }

    QChar peekQ() const {
        return data[pos];
    }

    bool next() {
        if (pos < data.size()-1) {
            ++pos;
            return true;
        }
        return false;
    }

    QVariant parse() {
        QVariant ret;

        skipWhitespace();
        switch (peek()) {
        case '{':
            ret = parseHash();
            break;
        case '[':
            ret = parseList();
            break;
        case '"':
            ret = parseString();
            break;
        case 't':
        case 'f':
            ret = parseBoolean();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            ret = parseNumber();
            break;
        default:
            ret = parseUnknown();
            break;
        }

        return ret;
    }

    QVariantHash parseHash() {
        bool hascomma = true;
        QVariantHash ret;

        Q_ASSERT(peek() == '{');
        next();
        do {
            skipWhitespace();

            QString key = parseString();
            skipWhitespace();
            Q_ASSERT(peek() == ':');
            next();
            QVariant value = parse();

            ret[key] = value;

            skipWhitespace();
            if (peek() == ',') {
                next();
            }
            else {
                hascomma = false;
            }
        } while (hascomma);
        skipWhitespace();
        //Q_ASSERT(peek() == '}');
        next();

        return ret;
    }

    QVariantList parseList() {
        bool hascomma = true;
        QVariantList ret;

        Q_ASSERT(peek() == '[');
        next();
        do {
            skipWhitespace();
            QVariant value = parse();

            ret << value;

            skipWhitespace();
            if (peek() == ',') {
                next();
            }
            else {
                hascomma = false;
            }
        } while (hascomma);
        skipWhitespace();
        //Q_ASSERT(peek() == ']');
        next();

        return ret;
    }

    QString parseString() {
        QString ret;

        Q_ASSERT(peek() == '"');
        while (true) {
            if (!next()) return ret;
            char c = peek();
            switch (c) {
            case '"':
                next();
                return ret;
            case '\\':
                next();
                // fall-through
            default:
                ret += peekQ();
            }
        }
    }

    QVariant parseBoolean() {
        bool retbool = detectString("true");
        pos += retbool ? 4 : 5;
        return retbool;
    }

    QVariant parseNumber() {
        QRegExp r("[^-0-9.e+]");

        int newPos = r.indexIn(data, pos, QRegExp::CaretAtOffset);
        QVariant ret(data.mid(pos, newPos-pos).toDouble());
        pos = newPos;
        return ret;
    }

    QVariant parseUnknown() {
        QVariant ret(QString("unknown(")+peekQ()+"|"+QString::number((int)peekQ().unicode())+")");
        next();
        return ret;
    }
};

#endif // JSONPARSER_H
