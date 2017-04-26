#ifndef SHAPES_H
#define SHAPES_H

#include <QImage>
#include <qpainter.h>
#include <algorithm>
#include <Filehandler/saveable.h>

#include "opencv2/opencv.hpp"

enum SHAPES {RECTANGLE, CIRCLE, LINE, ARROW, PEN, TEXT};

class Shape : Saveable {

public:
    Shape(QColor col, QPoint pos);
    void update_drawing_pos(QPoint pos);
    virtual void handle_new_pos(QPoint pos) = 0;
    virtual cv::Mat draw(cv::Mat &frame) = 0;

    void read(const QJsonObject& json);
    virtual void read_shape(const QJsonObject& json) = 0;
    void write(QJsonObject& json);
    virtual void write_shape(QJsonObject& json) = 0;

    static cv::Scalar qcolor_to_scalar(QColor col);
    static cv::Point qpoint_to_point(QPoint pnt);

    static const int LINE_THICKNESS = 2; // Constant used for the thickness of the drawn shapes.
    static constexpr double ALPHA = 0.6; // Costant used for the opacity.

protected:
    cv::Scalar colour;
    cv::Point draw_start;
    cv::Point draw_end;
};

#endif // SHAPES_H
