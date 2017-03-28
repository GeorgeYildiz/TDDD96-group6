#ifndef VIDEO_PRODUCER_H
#define VIDEO_PRODUCER_H

#include <QThread>
#include <QSemaphore>
#include "video_player.h"

class VideoProducer : public QThread { Q_OBJECT
public:
    VideoProducer(VideoPlayer* player, QSemaphore* load_video_sema, std::string file_name);
    double get_frame_rate();
signals:
    void fetched_frame(QImage frame);

public slots:

protected:
    void run() override;
private:
    VideoPlayer* m_player;
    std::string m_file_name;
    QSemaphore* m_load_sema;
    cv::VideoCapture capture;
};

#endif // VIDEO_PRODUCER_H
