#include <QString>
#include <QtTest>

#include "../src/LSystem.h"
#include "../src/VirtualTurtle.h"

class LSystemUnitTest : public QObject
{
    Q_OBJECT

public:
    LSystemUnitTest();

private Q_SLOTS:
    void iterationTest_data();
    void iterationTest();
    void virtualTurtleTest();
};

LSystemUnitTest::LSystemUnitTest()
{
}

void LSystemUnitTest::iterationTest_data()
{
    // axiom & rules
    QTest::addColumn<QString>("axiom");
    QTest::addColumn<RulesDict>("rules");
    // iteration results
    QTest::addColumn<QString>("state_0");
    QTest::addColumn<QString>("state_1");
    QTest::addColumn<QString>("state_2");
    QTest::addColumn<QString>("state_3");

    RulesDict rules;
    rules['A'] = "AB", rules['B'] = "A";
    QTest::newRow("Algea") << "A" << rules << "A" << "AB" << "ABA" << "ABAAB";

    rules.clear();
    rules['F'] = "G-F-G", rules['G'] = "F+G+F";
    QTest::newRow("Sierpinski") << "F" << rules << "F" << "G-F-G"
                                << "F+G+F-G-F-G-F+G+F"
                                << "G-F-G+F+G+F+G-F-G-F+G+F-G-F-G-F+G+F-G-F-G+F+G+F+G-F-G";
}

void LSystemUnitTest::iterationTest()
{
    QFETCH(QString, axiom);
    QFETCH(RulesDict, rules);
    LSystem lsystem(LSystem::string_to_state(axiom), rules, 0);

    QFETCH(QString, state_0);
    QCOMPARE(lsystem.state(), state_0.toStdString());

    QFETCH(QString, state_1);
    lsystem.iterate();
    QCOMPARE(lsystem.state(), state_1.toStdString());

    QFETCH(QString, state_2);
    lsystem.iterate();
    QCOMPARE(lsystem.state(), state_2.toStdString());

    QFETCH(QString, state_3);
    lsystem.iterate();
    QCOMPARE(lsystem.state(), state_3.toStdString());
}

namespace QTest {
    template<>
    char *toString(const QVector2D &vector)
    {
        QByteArray ba = "QVector2D[";
        ba += QByteArray::number(vector.x()) + ", ";
        ba += QByteArray::number(vector.y()) + ")";
        return qstrdup(ba.data());
    }
}

// comparing QPointF is a fuzzy comparison due to the floats and is thus
// likely to fail ; this is a sufficient test :
#define COMPARE_QPOINTF(a, b) QVERIFY((a-b).manhattanLength() < 1e-5)

void LSystemUnitTest::virtualTurtleTest()
{
    VirtualTurtle turtle(QPointF(50, 30));
    QCOMPARE(turtle.pos, QPointF(50, 30));

    turtle.pos = QPointF();
    turtle.heading = 90;
    turtle.forward(6.3f);
    COMPARE_QPOINTF(turtle.pos, QPointF(0, 6.3f));

    turtle.heading = 180;
    turtle.forward(5.f);
    COMPARE_QPOINTF(turtle.pos, QPointF(-5.f, 6.3f));
}

QTEST_APPLESS_MAIN(LSystemUnitTest)

#include "tst_lsystemunittest.moc"
