#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include <QThread>
#include "video_controller.h"
#include "video_producer.h"


class VideoThread : public QThread {
public:
    VideoThread(VideoController* controller, VideoPlayer* player);

protected:
 void run() override;

private:
    VideoController* m_controller;
    VideoPlayer* player;
};

#endif // VIDEO_THREAD_H
