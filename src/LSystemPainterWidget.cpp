#include "LSystemPainterWidget.h"

#include <QWidget>
#include <QPainter>
#include <QtDebug>

QBrush background_brush = QBrush(QColor(255, 255, 240)); // ivory color
QPen text_pen = QPen(Qt::black);
QPen path_pen = QPen(Qt::black);

LSystemPainterWidget::LSystemPainterWidget(LSystemPtr lsystem, QWidget *parent):
    LSystemRendererWidgetBase(lsystem, parent), m_pixmapSize()
{

}

LSystemPainterWidget::~LSystemPainterWidget()
{

}

void LSystemPainterWidget::paintEvent(QPaintEvent *)
{
    // create the painter and paint the background
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    // before first draw : print waiting message
    if (m_pixmap.isNull())
    {
        painter.setPen(text_pen);
        painter.drawText(rect(), Qt::AlignCenter,
                         tr("Rendering initial image, please wait..."));
        return;
    }

    // properly center the pixmap
    painter.save();
    //painter.translate(width() / 2 - m_pixmap.width() / 2, 0);

    // enable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.drawPixmap(rect(), m_pixmap, m_pixmap.rect());

    painter.restore();
}

void LSystemPainterWidget::resizeEvent(QResizeEvent *event)
{
    return QWidget::resizeEvent(event);
}

/**
 * @brief Debug tool. Make the given painter draw the coordinates system.
 */
void drawCoordinates(const QRect &rect, QPainter &painter)
{
    painter.setPen(Qt::red);

    painter.drawLine(0, 0, rect.width() / 2, 0);
    painter.drawText(60 - rect.width() / 2,
                     0 + rect.height() / 2, "x");

    painter.drawLine(0, 0, 0, rect.height() / 2);
    painter.drawText(0 - rect.width() / 2,
                     60 + rect.height() / 2, "y");
}

void LSystemPainterWidget::post_turtle_drawing()
{
    qDebug() << "post_turtle_drawing : pixmapOffset =" << m_pixmapOffset;


    // create a painter to the pixmap and draw the background
    QPainter painter(&m_pixmap);
    painter.fillRect(m_pixmap.rect(), background_brush);

    // enable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, true);

    QTransform transform;
    // we want to use the cartesian system
    // which means the Y-axis must be flipped
    // and we must also set up the correct scale
    painter.save();
    const float fwd = LSystemRendererWidgetBase::compute_drawing_distance();
    const float scale_factor = fwd / default_forward_distance;
    transform.scale(scale_factor, -scale_factor);
    //m.translate(0, -m_pixmap.height());
    transform.translate(-m_pixmapOffset.x(), -m_pixmapOffset.y());
    painter.setWorldTransform(transform, false);

    // do the actual rendering
    // i.e. draw all the paths to our pixmap
    painter.setBrush(background_brush);

    painter.setPen(path_pen);
    painter.drawPath(m_pixmapPainterPath);

    // memorize the scale of the rendered pixmap
    // the simplest way to do so is to save its size
    m_pixmapSize = m_pixmap.size();

    // debug : draw the coordinates system
    drawCoordinates(m_pixmap.rect(), painter);

    painter.restore();


    // mark the whole widget as 'dirty' (to be completely redrawn)
    update();
}

void LSystemPainterWidget::post_boundaries_computing()
{
    LSystemRendererWidgetBase::post_boundaries_computing();

    // set up and launch the rendering to the pixmap
    m_drawing = true;
    m_pixmapPainterPath = QPainterPath(m_turtle.pos);
    m_pixmap = QPixmap(size());
    m_pixmapOffset = m_boundaries.bottomLeft();
    emit start_processing();
}

void LSystemPainterWidget::turtle_pop()
{
    LSystemRendererWidgetBase::turtle_pop();
    m_pixmapPainterPath.moveTo(m_turtle.pos);
}

void LSystemPainterWidget::turtle_forward(float distance)
{
    // make our virtual turtle move
    m_turtle.forward(distance);

    // draw the movement we just did as a line
    m_pixmapPainterPath.lineTo(m_turtle.pos);
}

