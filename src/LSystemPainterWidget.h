#ifndef LSYSTEMPAINTERWIDGET_H
#define LSYSTEMPAINTERWIDGET_H

#include "LSystemRendererWidgetBase.h"

#include <QPixmap>
#include <QPainterPath>

class QPaintEvent;
class QResizeEvent;

/**
 * @brief This widget renders L-Systems using Qt's QPainter graphics system.
 *
 * Following the Qt Mandelbrot example, it does all its rendering into a cache
 * texture (a QPixmap) wich allows this widget to be responsive.
 *
 * For instance when resizing this widget the scaled pixmap
 * will be displayed until a new one is rendered.
 *
 * Internally, uses QPainterPath to draw all the lines in one call (downside :
 * no color possible...).
 */
class LSystemPainterWidget : public LSystemRendererWidgetBase
{
public:
    LSystemPainterWidget(LSystemPtr lsystem, QWidget *parent = 0);
    ~LSystemPainterWidget();

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    void post_turtle_drawing() Q_DECL_OVERRIDE;
    void post_boundaries_computing() Q_DECL_OVERRIDE;

    void turtle_pop() Q_DECL_OVERRIDE;

private:
   /**
    * @brief Overrides LSystemRendererWidgetBase::turtle_forward to draw the
    * required lines.
    * @param distance Forward distance to be drawn from the VirtualTurtle's
    * current position.
    */
   virtual void turtle_forward(float distance) Q_DECL_OVERRIDE;

    QPixmap m_pixmap;       //!< offscreen paint device acting as a rendering cache
    QPointF m_pixmapOffset; //!< origin offset used in the rendering of the pixmap
    QPainterPath m_pixmapPainterPath;
    QSize m_pixmapSize;   //!< size of the rendered pixmap
};

#endif /* LSYSTEMPAINTERWIDGET_H */
