#include "video_thread.h"
#include "video_player.h"
#include <QEventLoop>
#include <QSemaphore>
VideoThread::VideoThread(VideoController* controller, VideoPlayer* p) {
    m_controller = controller;
    player = p;
}

void VideoThread::run() {
    player->open_video();
    QEventLoop loop;

    QObject::connect(m_controller, SIGNAL(play()), player, SLOT(play()));
    QObject::connect(m_controller, SIGNAL(pause()), player, SLOT(pause()));
    QObject::connect(m_controller, SIGNAL(stop()), player, SLOT(stop()));

    double frame_rate = player->get_frame_rate();
    int delay = (1000/frame_rate);
    std::cout << "Delay: " << delay << std::endl;
    while(!player->is_stopped()) {
        if (player->is_playing()) {
            player->emit_next_frame();
            this->msleep(delay);
        }
        loop.processEvents();
    }
}
