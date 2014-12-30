#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <QObject>
#include <QMap>
#include <list>
#include <QMutexLocker>


/**
 * @brief State is the container for an L-System's state.
 */
typedef std::string State;

/**
* @brief RulesDict is a QMap of the type {variable_char:producted_string}
* representing the production rules of an L-System.
* Utilizes a QString as a producted string for convenience.
*/
typedef QMap<char, QString> RulesDict;

/**
 * @brief Implements a simple Lindenmayer System, or L-System.
 *
 * Inheritance from QObject allows this class to work in another thread and
 * to use signals and slots.
 *
 * It consists in a string of characters (the state), a seed (the axiom) and
 * a collection of production (or transition, growing...) rules.
 * For instance with the Sierpinski Triangle :
 * - 'F' and 'G' the two variables
 * - "F" our initial axiom (i.e. the seed)
 * - "F=G-F-G" and "G=F+G+F" our production rules.
 * the first iteration will give "G-F-G", the second "F+G+F-G-F-G-F+G+F...
 *
 * A variable need to be a single ASCII character : this speeds up
 * the iteration algorithm, reduces the needed memory and avoids any abiguous production rules.
 *
 * @todo Use compression to reduce memory usage by the State string ?
 * @todo iterate() using a variable-sized buffer : allows for cancelling
 */
class LSystem : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor for LSystem.
     * @param rules The RulesDict containing the production rules.
     * @param axiom Axiom of the L-System (its initial state, or seed).
     * @param parent Parent of the LSystem as a QObject.
     */
    explicit LSystem(const State &axiom, const RulesDict &rules,
                     QObject *parent = 0);
    ~LSystem();

    /**
     * @brief Iterate (or, biologically speaking, evolve) the system to its next
     * state according to its production (evolution) rules. Thread-safe.
     */
    void iterate();

    /**
     * @brief Thread-safe accessor for the current generation number.
     * @return The current generation number.
     */
    uint generation() const { QMutexLocker locker(&m_mutex); return m_N; }

    /**
     * @brief Thread-safe accessor for the current state.
     * @return A reference to the L-System state.
     */
    const State &state() const { QMutexLocker locker(&m_mutex); return m_state; }

    /**
     * @brief Return the given string as a State.
     */
    static State string_to_state(const QString &string);

signals:
    /**
     * @brief When iterating (see iterate()), fired whenever a progress is made.
     * @param percentage The percentage of the work done (< 100%).
     */
    void iteration_progressed(unsigned int percentage);

    /**
     * @brief Called when an iteration work is finished.
     */
    void iteration_finished();

private:
    mutable QMutex m_mutex;
    State m_state;
    RulesDict m_rules;
    uint m_N;
};

#endif /* LSYSTEM_H */
