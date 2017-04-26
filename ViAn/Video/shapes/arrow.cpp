#include "arrow.h"

/**
 * @brief Arrow::Arrow
 * @param col Colour of the new object
 * @param pos Starting point for the new object
 */
Arrow::Arrow(QColor col, QPoint pos) : Shape(col, pos) {
}

/**
 * @brief Arrow::draw
 * Draws the object on top of the specified frame.
 * @param frame Frame to draw on.
 * @return Returns the frame with drawing.
 */
cv::Mat Arrow::draw(cv::Mat &frame) {
    cv::arrowedLine(frame, draw_start, draw_end, colour, LINE_THICKNESS);
    return frame;
}

/**
 * @brief Arrow::handle_new_pos
 * Function to handle the new position of the mouse.
 * Does not need to store the new position.
 * @param pos
 */
void Arrow::handle_new_pos(QPoint pos) {
}

/**
 * @brief Arrow::read_shape
 * Stores specific information associated
 * @param json
 */
void Arrow::read_shape(const QJsonObject &json) {
}

/**
 * @brief Arrow::write_shape
 * @param json
 */
void Arrow::write_shape(QJsonObject &json) {
}
