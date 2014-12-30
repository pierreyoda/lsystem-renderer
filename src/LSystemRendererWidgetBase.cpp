#include "LSystemRendererWidgetBase.h"

#include "LSystem.h"
#include <QtDebug>

const float LSystemRendererWidgetBase::default_forward_distance = 10.f;
const int processor_update_step = 5; // update only every x % (0 <= x <= 100)

LSystemRendererWidgetBase::LSystemRendererWidgetBase(LSystemPtr lsystem,
                                                     QWidget *parent) :
    QWidget(parent),
    m_turtle(QPointF(0.f, 0.f)), m_lsystem(lsystem), m_emptyState(),
    m_processingThread(), m_processingTimer(), m_turtle_stack()
{
    m_forward_distance = default_forward_distance;
    m_rotation_angle = 20.f;

    m_drawing = false;
    m_generation_processed = -1;

    // create the worker and move it to the working thread
    m_processor = new LSystemProcessor(*this);
    m_processor->moveToThread(&m_processingThread);

    // do the necessary connections
    connect(&m_processingThread, &QThread::finished,
            m_processor, &QObject::deleteLater);
    connect(this, &LSystemRendererWidgetBase::start_processing,
            m_processor, &LSystemProcessor::process);
    connect(m_processor, &LSystemProcessor::progressed,
            this, &LSystemRendererWidgetBase::processor_progressed);
    connect(m_processor, &LSystemProcessor::finished,
            this, &LSystemRendererWidgetBase::processor_finished);
    connect(m_processor, &LSystemProcessor::error,
            this, &LSystemRendererWidgetBase::processor_error);

    // start the processing thread
    m_processingThread.start();
}

LSystemRendererWidgetBase::~LSystemRendererWidgetBase()
{
    m_processingThread.quit();
    m_processingThread.wait();

    m_lsystem.clear();

    if (m_processor != nullptr)
        m_processor->deleteLater();  // the deleteLater connection should do it anyway
}

void LSystemRendererWidgetBase::render_lSystem()
{
    if (m_lsystem.isNull())
        return;

    process_lsystem();
}

void LSystemRendererWidgetBase::process_lsystem()
{
    // security check (which should never be triggered)
    if (m_lsystem.isNull())
            return;

    // start the processing worker to find the bounding box, if needed
    if (m_generation_processed == static_cast<int>(m_lsystem->generation()))
    {
        post_boundaries_computing();
        return;
    }

    m_drawing = false;
    turtle_reset();
    m_forward_distance = default_forward_distance;
    //emit status_changed("Computing boundaries...");
    m_processingTimer.start();
    emit start_processing();
}

void LSystemRendererWidgetBase::processor_progressed(uint progress)
{
    emit progress_changed(progress);
}

void LSystemRendererWidgetBase::processor_finished()
{
    QString status = tr("%2 done in %1 ms").arg(m_processingTimer.elapsed());
    // if finished drawing :
    if (m_drawing)
    {
        emit status_changed(status.arg(tr("Rendering")));
        post_turtle_drawing();
    }
    // otherwise, we just finished a virtual draw
    else
    {
        emit status_changed(status.arg(tr("Computing boundaries")));
        post_boundaries_computing();
    }
}

void LSystemRendererWidgetBase::processor_error(const QString &error)
{
    emit status_changed(error);
}

void LSystemRendererWidgetBase::post_boundaries_computing()
{
    m_generation_processed = m_lsystem->generation();

    turtle_reset();
    m_processingTimer.start();

    // also ensure the progress for that job is 100%
    emit progress_changed(100);
}


float LSystemRendererWidgetBase::compute_drawing_distance()
{
    qDebug() << "boundaries rect : " << m_boundaries;

    // 1) Scale our forward distance
    // we scale to the axis with the biggest delta (max-min)
    if (m_boundaries.width() > m_boundaries.height())
        return width() * default_forward_distance / m_boundaries.width();
    else
        return height() * default_forward_distance / m_boundaries.height();
}

void LSystemRendererWidgetBase::turtle_forward(float distance)
{
    // by default : do not draw anything
    m_turtle.forward(distance);
}

void LSystemRendererWidgetBase::turtle_push()
{
    m_turtle_stack.append(TurtleState(m_turtle.pos, m_turtle.heading));
}

void LSystemRendererWidgetBase::turtle_pop()
{
    const TurtleState state = m_turtle_stack.pop();
    m_turtle.pos = state.first, m_turtle.heading = state.second;
}

void LSystemRendererWidgetBase::turtle_reset()
{
    m_turtle.pos = QPointF();
    m_turtle.heading = 90.f; // default heading = north (logo-style)
    m_turtle_stack.clear();
}


LSystemProcessor::LSystemProcessor(LSystemRendererWidgetBase &master,
                                   QObject *parent) : QObject(parent),
    m_master(master)
{

}

void LSystemProcessor::process()
{
    const bool drawing = m_master.m_drawing;

    // if computing boundaries : remember these values
    float minX = 0, minY = 0, maxX = 0, maxY = 0;

    // the total work to be done is approximatively known
    const State &state = m_master.m_lsystem->state();
    const int lenght = state.length();
    int i = 0, last_progress_sent = -processor_update_step;

    // iterate the characters in the current state
    bool valid = true;
    QString error_string;
    State::const_iterator it;
    for (it = state.begin(); valid && it != state.end(); ++it)
    {
        switch(*it)
        {
            case '+':
                m_master.m_turtle.left(m_master.m_rotation_angle);
                break;
            case '-':
                m_master.m_turtle.right(m_master.m_rotation_angle);
                break;
            case '[':
                m_master.turtle_push();
                break;
            case ']':
                if (m_master.m_turtle_stack.isEmpty())
                {
                    error_string = "LSystemProcessor error : cannot pop empty turtle stack";
                    valid = false;
                }
                else
                    m_master.turtle_pop();
                break;
            // todo : handle alphabet properly
            case 'F':
                m_master.turtle_forward(m_master.m_forward_distance);
                break;
        }

        // if computing boundaries : register the extrema values
        if (!drawing)
        {
            const float x = m_master.m_turtle.pos.x(),
                    y = m_master.m_turtle.pos.y();
            if (x < minX)
                minX = x;
            else if (x > maxX)
                maxX = x;
            if (y < minY)
                minY = y;
            else if (y > maxY)
                maxY = y;
        }

        // update our progress with an approximation of the work done
        // in reality not all jobs take the same amount of time
        const int progress = 100 * ++i / lenght;
        // but avoid sending too many signals (it's quite slow with very long states)
        if (progress == 100 || (progress-last_progress_sent) >= processor_update_step)
        {
            last_progress_sent = progress;
            emit progressed(progress);
        }
    }

    // handle possible error
    if (!valid)
    {
        emit error(error_string);
        return;
    }

    // if virtual draw : we now know our boundaries
    if (!drawing)
        m_master.m_boundaries = QRectF(QPointF(minX, minY), QPointF(maxX, maxY));

    emit finished();
}
