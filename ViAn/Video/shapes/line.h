#ifndef LINE_H
#define LINE_H

#include "shape.h"

class Line : public Shape {
public:
    Line(QColor col, QPoint pos);
    cv::Mat draw(cv::Mat &frame) override;
    void handle_new_pos(QPoint pos) override;
    void write(QJsonObject& json) override;
};

#endif // LINE_H
