#include "LSystem.h"

LSystem::LSystem(const State &axiom, const RulesDict &rules,
                 QObject *parent) : QObject(parent), m_mutex(),
    m_state(axiom), m_rules(rules), m_N(0)
{


}

LSystem::~LSystem()
{

}

void LSystem::iterate()
{
    m_mutex.lock();
    State newState;
    State::const_iterator iter;
    int i = 0;
    const int L = m_state.length();
    for (iter = m_state.begin(); iter != m_state.end(); ++iter)
    {
        // if the character is not in the production rules
        // it's a constant, hence the default value
        const char c = *iter;
        const QString c_product = m_rules.value(c, QChar(c));

        // we do a 'manual append' to avoid bad_alloc exceptions
        newState += c_product.toStdString();

        // update our progress
        emit iteration_progressed(100 * ++i /L );
    }

    m_state = newState, ++m_N;
    m_mutex.unlock();

    emit iteration_finished();
}

State LSystem::string_to_state(const QString &string)
{
    return State(string.toStdString());
}

