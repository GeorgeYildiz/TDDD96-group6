#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QObject>
#include <QImage>
#include <iostream>
#include <queue>
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

class VideoPlayer : public QObject { Q_OBJECT
public:
    explicit VideoPlayer(std::string file_name, QObject *parent = 0);
    bool is_stopped();
    bool is_playing();
    bool is_buffer_empty();
    void emit_next_frame();
    double get_frame_rate();
    void open_video();

signals:
    void display(QImage frame);

public slots:                                  
    void play();
    void pause();
    void stop();

private:
    bool m_is_playing = true;
    bool m_is_stopped = false;
    std::string m_file_name;
    cv::VideoCapture* capture;
    ~VideoPlayer();
};

#endif // VIDEO_PLAYER_H
