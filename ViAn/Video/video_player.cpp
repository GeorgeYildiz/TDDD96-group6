#include "video_player.h"

video_player::video_player(QObject *parent) : QObject(parent) {

}

void video_player::play() {
    mIsPlaying = true;
}

void video_player::pause() {
    mIsPlaying = false;
}

void video_player::stop() {
    mIsStopped = true;
}

void video_player::enqueueFrame(QImage frame) {
    std::cout << "Buffering frame.." << std::endl;
    buffer.push_back(frame);
    std::cout << buffer.size() << std::endl;

}

bool video_player::is_stopped() {
    return mIsStopped;
}

bool video_player::is_buffer_empty() {
    return buffer.size() == 0;
}

void video_player::emit_next_frame() {
    emit display(buffer.pop_front());
}
