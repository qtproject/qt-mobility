#include <QObject>
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "koreaninput.h"

KoreanInput* mykoreaninput;
QStringList testlist;
QString teststring;

class UT_easydial_koreaTest : public QObject
{
    Q_OBJECT
    
public:
    UT_easydial_koreaTest();
    ~UT_easydial_koreaTest();

private Q_SLOTS:

    void testTokenize_emptystring();
    void testTokenize_findConsonant_01();
    void testTokenize_findConsonant_02();
    void testTokenize_findVowel_3_01();
    void testTokenize_findVowel_3_02();
    void testTokenize_findVowel_6_01();
    void testTokenize_findVowel_6_02();

    void testTokenize_misc_01();
    void testTokenize_misc_02();

    void testCountConsecutive_01();
};


UT_easydial_koreaTest::UT_easydial_koreaTest()
{
    mykoreaninput = new KoreanInput();
}

UT_easydial_koreaTest::~UT_easydial_koreaTest()
{
    delete mykoreaninput;
}

void UT_easydial_koreaTest::testTokenize_emptystring()
{
    teststring = QString("");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY2 (testlist.count() == 0, "token list was not empty");
    testlist.clear();
}


void UT_easydial_koreaTest::testTokenize_findConsonant_01()
{
    //test tokenizing first consonant of input string

    teststring = QString("1*2*#8*5");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY (testlist.at(0)=="1*");
    testlist.clear();
}


void UT_easydial_koreaTest::testTokenize_findConsonant_02()
{
    //Test tokenizing four first consonants of input string

    teststring = QString("1*2*#8*5");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY (testlist.at(0)=="1*");
    QVERIFY (testlist.at(1)=="2*#");
    QVERIFY (testlist.at(2)=="8*");
    QVERIFY (testlist.at(3)=="5");
    testlist.clear();

}


void UT_easydial_koreaTest::testTokenize_findVowel_3_01()
{
    //tokenize the vowel 3 under ITU-T 12-key keypad
    //simple, correct vowels
    QVERIFY (mykoreaninput->Tokenize("3").at(0) == "3");
    QVERIFY (mykoreaninput->Tokenize("3*").at(0) == "3*");
    QVERIFY (mykoreaninput->Tokenize("3*9").at(0) == "3*9");

}


void UT_easydial_koreaTest::testTokenize_findVowel_3_02()
{
    //find the vowel under ITU-T 12-key keypad number '3'

    //multiple  vowels in string:
    teststring = QString("3***393**393");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY (testlist.at(0)=="3***");
    QVERIFY (testlist.at(1)=="39");
    QVERIFY (testlist.at(2)=="3**39");
    QVERIFY (testlist.at(3)=="3");
    testlist.clear();
}

void UT_easydial_koreaTest::testTokenize_findVowel_6_01()
{
    //tokenize the vowel under ITU-T 12-key keypad number '6'
    //simple, correct vowels
    QVERIFY (mykoreaninput->Tokenize("6").at(0) == "6");
    QVERIFY (mykoreaninput->Tokenize("6*").at(0) == "6*");
    QVERIFY (mykoreaninput->Tokenize("63").at(0) == "63");
    QVERIFY (mykoreaninput->Tokenize("639").at(0) == "639");
    QVERIFY (mykoreaninput->Tokenize("6*9").at(0) == "6*9");
}

void UT_easydial_koreaTest::testTokenize_findVowel_6_02()
{
    //tokenize the vowel under ITU-T 12-key keypad number '6'
    //multiple  vowels in string:
    teststring = QString("6396*96966");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY (testlist.at(0)=="639");
    QVERIFY (testlist.at(1)=="6*9");
    QVERIFY (testlist.at(2)=="69");
    QVERIFY (testlist.at(3)=="66");
    testlist.clear();
}


void UT_easydial_koreaTest::testTokenize_misc_01()
{
    //tokenize string containing misc vowels and consonants
    teststring = QString("6396*96966");
    testlist = mykoreaninput->Tokenize(teststring);
    QVERIFY (testlist.at(0)=="639");
    QVERIFY (testlist.at(1)=="6*9");
    QVERIFY (testlist.at(2)=="69");
    QVERIFY (testlist.at(3)=="66");
    testlist.clear();
}

void UT_easydial_koreaTest::testTokenize_misc_02()
{
    //tokenize string containing extra chars and multiple
    //presses of '#' and '*' keys, simulating user input

    testlist = mykoreaninput->Tokenize("1**3#*#*6##93**2");
    QVERIFY (testlist.at(0)=="1**");
    QVERIFY (testlist.at(1)=="3#*#*");
    QVERIFY (testlist.at(2)=="6##9");
    QVERIFY (testlist.at(3)=="3**");
    testlist.clear();

    testlist = mykoreaninput->Tokenize("**6***###09*");
    QVERIFY (testlist.at(0)=="**");
    QVERIFY (testlist.at(1)=="6***###");
    QVERIFY (testlist.at(2)=="09");
    QVERIFY (testlist.at(3)=="*");
    testlist.clear();

    testlist = mykoreaninput->Tokenize("3***3******36****390");
    QVERIFY (testlist.at(0)=="3***");
    QVERIFY (testlist.at(1)=="3******3");
    QVERIFY (testlist.at(2)=="6****39");
    QVERIFY (testlist.at(3)=="0");
    testlist.clear();

    testlist = mykoreaninput->Tokenize("######3*96****39777");
    QVERIFY (testlist.at(0)=="######");
    QVERIFY (testlist.at(1)=="3*9");
    QVERIFY (testlist.at(2)=="6****39");
    QVERIFY (testlist.at(3)=="7");
    testlist.clear();

    testlist = mykoreaninput->Tokenize("**6***3**6#0");
    QVERIFY (testlist.at(0)=="**");
    QVERIFY (testlist.at(1)=="6***");
    QVERIFY (testlist.at(2)=="3**");
    QVERIFY (testlist.at(3)=="6#");
    testlist.clear();

}



void UT_easydial_koreaTest::testCountConsecutive_01()
{
    //test the counting function for consecutive characters
    QVERIFY (mykoreaninput->countConsecutive("#####***asddd",'#',0) == 5);
    QVERIFY (mykoreaninput->countConsecutive("sasd***asddd",'*',4) == 3);
    QVERIFY (mykoreaninput->countConsecutive("abcdXefgh",'X',4) == 1);
    QVERIFY (mykoreaninput->countConsecutive("1234***4567#",'1',3) == 0);
    QVERIFY (mykoreaninput->countConsecutive("1234***4567#",'#',11) == 1);

    QVERIFY (mykoreaninput->countConsecutive("",'8',25) == 0);
    QVERIFY (mykoreaninput->countConsecutive("1",'6',-1) == 0);

}



QTEST_APPLESS_MAIN(UT_easydial_koreaTest);

#include "tst_ut_easydial_koreatest.moc"
