#include "video_thread.h"
#include "video_player.h"
#include <QEventLoop>
video_thread::video_thread(video_controller* controller) {
    mController = controller;
}

void video_thread::run() {
    QEventLoop loop;
    video_player* player = new video_player;
    video_producer* producerThread = new video_producer(player);
    producerThread->start();

    //QObject::connect(producerThread, SIGNAL(fetched_frame(QImage frame)), player, SLOT(enqueueFrame(QImage frame)));
    QObject::connect(mController, SIGNAL(play()), player, SLOT(play()));
    QObject::connect(mController, SIGNAL(pause()), player, SLOT(pause()));
    QObject::connect(mController, SIGNAL(stop()), player, SLOT(stop()));

    while(!player->is_stopped()) {
        if (player->is_playing() && !player->is_buffer_empty()) {
            player->emit_next_frame();
        }
        loop.processEvents();
    }
}
