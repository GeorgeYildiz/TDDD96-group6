#include "video_player.h"

VideoPlayer::VideoPlayer(std::string file_name, QObject *parent) : QObject(parent) {
    m_file_name = file_name;
}

void VideoPlayer::open_video() {
    capture = new cv::VideoCapture(m_file_name);
}

void VideoPlayer::emit_next_frame() {
    QImage img;
    cv::Mat frame, RGBframe;
    if (capture->read(frame)) {
        if (frame.channels()== 3) {
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        } else {
            img = QImage((const unsigned char*)(frame.data),
                                 frame.cols,frame.rows,QImage::Format_Indexed8);
            //std::cout << "Frame size: " << img.byteCount() << std::endl;
        }
        emit display(img);
    } else {
        m_is_stopped = true;
    }
}

void VideoPlayer::play() {
    m_is_playing = true;
}

void VideoPlayer::pause() {
    m_is_playing = false;
}

void VideoPlayer::stop() {
    m_is_stopped = true;
}


bool VideoPlayer::is_playing() {
    return m_is_playing;
}

bool VideoPlayer::is_stopped() {
    return m_is_stopped;
}

double VideoPlayer::get_frame_rate() {
    std::cout << "Video opened: " << capture->isOpened() << ", fps: " << capture->get(CV_CAP_PROP_FPS) << std::endl;
    return capture->get(CV_CAP_PROP_FPS);
}

VideoPlayer::~VideoPlayer() {
    capture->release();
    delete capture;
}
