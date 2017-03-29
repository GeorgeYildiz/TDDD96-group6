#ifndef TEXT_H
#define TEXT_H

#include "Shape.h"

class Text : public Shape {
public:
    Text(QColor col, QPoint pos, QString strng);
    void draw(QImage &img) override;
    void handle_new_pos(QPoint pos);
private:
    const int FONT_SIZE = 24;
    QString string;
};

#endif // TEXT_H