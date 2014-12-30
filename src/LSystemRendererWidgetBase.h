#ifndef LSYSTEMRENDERERWIDGETBASE_H
#define LSYSTEMRENDERERWIDGETBASE_H

#include <QWidget>
#include <QThread>
#include <QElapsedTimer>
#include <QStack>

#include "VirtualTurtle.h"

class LSystem;
class LSystemRendererWidgetBase;

/**
 * @brief Interpreter for the L-System state.
 *
 * Allows LSystemRendererWidgetBase::process_lsystem to be non-blocking.
 */
class LSystemProcessor : public QObject
{
    Q_OBJECT

public:
    explicit LSystemProcessor(LSystemRendererWidgetBase &master,
                              QObject *parent = 0);
    ~LSystemProcessor() {}

public slots:
    void process();

signals:
    void progressed(uint progress);
    void finished();
    void error(const QString &error);

protected:
    LSystemRendererWidgetBase &m_master;
};

/**
 * @brief A shared pointer to an L-System.
 */
typedef QSharedPointer<LSystem> LSystemPtr;

/**
 * @brief The LSystemRendererWidgetBase is the base widget for any
 * L-System visualization widget.
 *
 * No actual rendering is done by this virtual class.
 */
class LSystemRendererWidgetBase : public QWidget
{
    friend class LSystemProcessor;

    Q_OBJECT

public:
    /**
     * @brief Default constructor.
     * @param lsystem LSystemPtr to the LSystem to render.
     * @param parent Pointer to the widget's parent, if it exists.
     */
    explicit LSystemRendererWidgetBase(LSystemPtr lsystem, QWidget *parent = 0);
    ~LSystemRendererWidgetBase();

public slots:
    /**
     * @brief Call whenever the L-System needs to be (re)drawn, e.g. when
     * it has just been iterated.
     */
    void render_lSystem();

signals:
    /**
     * @brief Fired when the renderer's current status changes.
     * @param status The new status (textual).
     */
    void status_changed(const QString &status);

    /**
     * @brief When working, fired when progress has been made.
     * @param percentage The percentage of the work done (< 100%).
     */
    void progress_changed(uint percentage);

    /**
     * @brief Order the LSystemProcessor to start working.
     */
    void start_processing();

protected:
    /**
     * @brief Deduce from the bounding box found in the virtual drawing step
     * and from the widget size the forward drawing distance to use to fit all
     * the drawing inside the widget.
     */
    float compute_drawing_distance();

    /**
     * @brief Pure virtual function called after the rendering processing
     * step is done.
     */
    virtual void post_turtle_drawing() = 0;

    /**
     * @brief Virtual function called after the bounding fox was found
     * in the virtual drawing step, or directly in render_lSystem() if this step
     * did not need to be redone.
     * This is where the actual drawing should be started.
     *
     * Default behavior : resets the VirtualTurtle and the processing timer,
     * and updates the generation processed counter (see m_generation_processed).
     *
     * N.B. : overriding function should not do heavy computation in the main
     * thread to not freeze the GUI.
     */
    virtual void post_boundaries_computing();

    /**
     * @brief Make the turtle go forward
     * Virtual function that must be implemented by the child classes in order
     * to do any drawing.
     * Default behavior : make the turtle go forward by distance.
     *
     * N.B. : this function is called by LSystemProcessor during its work.
     *
     * @param distance Forward distance to be drawn from the VirtualTurtle's
     * current position.
     */
    virtual void turtle_forward(float distance);

    /**
     * @brief Push the turtle's state to the turtle states stack.
     */
    virtual void turtle_push();

    /**
     * @brief Restore from the stack the state of the VirtualTurtle.
     *
     * It is assumed here that m_turtle_stack is not empty.
     */
    virtual void turtle_pop();

    QRectF m_boundaries;
    VirtualTurtle m_turtle;
    static const float default_forward_distance; //!< used for computing boundaries
    float m_forward_distance;
    QPointF m_origin_offset;

    bool m_drawing; // if false and processing : currently computing boundaries

private slots:
    /**
     * @brief Called by LSystemProcessor to update its progress.
     * @param progress In percentage, from 0 to 100. At 100 : the processor finished.
     */
    void processor_progressed(uint progress);

    /**
     * @brief Called by LSystemProcessor when it finished processing the LSystem.
     */
    void processor_finished();

    /**
     * @brief Called by LSystemProcessor when it encountered an error.
     */
    void processor_error(const QString &error);

private:
    /**
     * @brief Translate the L-System's state into a series of commands for
     * the VirtualTurtle and execute them.
     *
     */
    void process_lsystem();

    void turtle_reset();

    QSharedPointer<LSystem> m_lsystem;
    QString m_emptyState;

    QThread m_processingThread;
    QElapsedTimer m_processingTimer;
    LSystemProcessor *m_processor;
    float m_rotation_angle;
    QStack<TurtleState> m_turtle_stack;
    /**
     * @brief Last processed generation (-1 if none).
     * Does not reflect the rendering part of the processing but only
     * the virtual draw step.
     */
    int m_generation_processed;
};

#endif /* LSYSTEMRENDERERWIDGETBASE_H */
