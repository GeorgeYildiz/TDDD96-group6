#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include <QThread>
#include "video_controller.h"
#include "video_producer.h"


class video_thread : public QThread {
public:
    video_thread(video_controller* controller);

protected:
 void run() override;

private:
    video_controller* mController;
};

#endif // VIDEO_THREAD_H
