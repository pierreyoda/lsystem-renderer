#ifndef VIRTUALTURTLE_H
#define VIRTUALTURTLE_H

#include <QPointF>
#include <QtMath>
#include <QPair>

/**
 * @brief Typedef for the container of a state of the turtle (position+heading).
 */
typedef QPair<QPointF, float> TurtleState;

/**
 * @brief VirtualTurtle implements the famous Turtle found in Python or Logo languages.
 *
 * No actual drawing is done, only the angle (heading) and the position is
 * stored.
 * Following logo's convention, the orientation of the angles is clockwise.
 */
struct VirtualTurtle
{
    VirtualTurtle(const QPointF &position) : pos(position), heading(0.f) { }

    /**
     * @brief Go forward by distance.
     */
    void forward(float distance)
    {
        float angle = qDegreesToRadians(heading);
        pos += distance * QPointF(qCos(angle), qSin(angle));
    }

    /**
     * @brief Rotate left by angle (counterclockwise if angle positive).
     */
    inline void left(float angle) { heading -= angle; }

    /**
     * @brief Rotate right by angle (clockwise if angle positive).
     */
    inline void right(float angle) { heading += angle; }

    QPointF pos;    //!< Turtle's position vector
    float heading;  //!< Turtle's heading (its angle with the origin)
};

#endif /* VIRTUALTURTLE_H */
