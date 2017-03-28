#include "video_producer.h"
#include <iostream>

VideoProducer::VideoProducer(VideoPlayer* player, QSemaphore* load_video_sema, std::string file_name) {
    m_player = player;
    m_file_name = file_name;
    m_load_sema = load_video_sema;
}



double VideoProducer::get_frame_rate() {
    std::cout << "Video opened: " << capture.isOpened() << ", fps: " << capture.get(CV_CAP_PROP_FPS) << std::endl;
    return capture.get(CV_CAP_PROP_FPS);
}
