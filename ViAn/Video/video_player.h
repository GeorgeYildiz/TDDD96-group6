#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QObject>
#include <QImage>
#include <iostream>
#include <list>
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

class video_player : public QObject { Q_OBJECT
public:
    explicit video_player(QObject *parent = 0);
    bool is_stopped();
    bool is_playing();
    bool is_buffer_empty();
    void emit_next_frame();

signals:
    void display(QImage frame);

public slots:                                  
    void play();
    void pause();
    void stop();
    void enqueueFrame(QImage frame);

private:
    bool mIsPlaying;
    bool mIsStopped;
    std::list<QImage> buffer;
};

#endif // VIDEO_PLAYER_H
