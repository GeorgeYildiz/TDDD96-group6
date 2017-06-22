#include "framewidget.h"
#include <QDebug>

FrameWidget::FrameWidget(QWidget *parent) : QWidget(parent) {
}

void FrameWidget::draw_from_playback(cv::Mat frame) {
    current_frame = frame;
    draw_image(frame);
}

void FrameWidget::toggle_zoom(bool value) {
    if (value) {
        tool = ZOOM;
        setCursor(Qt::CrossCursor);
    } else {
        unsetCursor();
        tool = NONE;
    }
}

void FrameWidget::draw_image(cv::Mat image) {
    // Convert the image to the RGB888 format
    switch (image.type()) {
        case CV_8UC1:
            cvtColor(image, _tmp, CV_GRAY2RGB);
            break;
        case CV_8UC3:
            cvtColor(image, _tmp, CV_BGR2RGB);
            break;
    }

    // QImage needs the data to be stored continuously in memory
    assert(_tmp.isContinuous());
    // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
    // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
    // has three bytes.
    _qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols*3, QImage::Format_RGB888);
    this->setFixedSize(image.cols, image.rows);
    repaint();
}

/**
 * @brief FrameWidget::paintEvent
 * @param event
 */
void FrameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), _qimage);

    if (draw_zoom_rect) {
        painter.setPen(QColor(0,255,0));
        QRectF zoom(zoom_start_pos, zoom_end_pos);
        painter.drawRect(zoom);
    }

    painter.end();
}

/**
 * @brief FrameWidget::resizeEvent
 * @param event
 */
void FrameWidget::resizeEvent(QResizeEvent *event) {
    emit current_size(width(), height());
}

/**
 * @brief FrameWidget::mousePressEvent
 * @param event
 */
void FrameWidget::mousePressEvent(QMouseEvent *event) {
    switch (tool) {
    case ZOOM:
        zoom_start_pos = event->pos();
        break;
    default:
        prev_pos = event->pos();
        break;
    }
}

/**
 * @brief FrameWidget::mouseReleaseEvent
 * @param event
 */
void FrameWidget::mouseReleaseEvent(QMouseEvent *event) {
    switch (tool) {
    case ZOOM: {
        draw_zoom_rect = false;
        repaint();

        int width = std::abs(zoom_start_pos.x() - zoom_end_pos.x());
        int height = std::abs(zoom_start_pos.y() - zoom_end_pos.y());
        double width_ratio = current_frame.cols / double(width );
        double height_ratio = current_frame.rows / double(height);
        emit zoom_factor(std::min(width_ratio, height_ratio));
        break;
    }
    default:
        break;
    }
}

/**
 * @brief FrameWidget::mouseMoveEvent
 * @param event
 */
void FrameWidget::mouseMoveEvent(QMouseEvent *event) {
    switch (tool) {
    case ZOOM:
        zoom_end_pos = event->pos();
        draw_zoom_rect = true;
        repaint();
        break;
    default:
        QPoint _tmp = prev_pos - event->pos();
        emit moved_xy(_tmp.x(), _tmp.y());
        prev_pos = event->pos();
        break;
    }
    if (do_zoom) {

    }
}




