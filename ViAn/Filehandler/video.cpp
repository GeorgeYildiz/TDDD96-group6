#include "video.h"
/**
 * @brief Video::Video
 */
video::video()
{
    this->filepath = "";
}
/**
 * @brief Video::Video
 * @param id
 * @param filepath
 */
video::video(std::string filepath){
    this->filepath = filepath;
}
/**
 * @brief operator >>
 * @param is
 * @param vid
 * @return UNFINISHED
 */
std::stringstream& operator>>(std::stringstream& is, video& vid){
    is >> vid.filepath;  
    return is;
}
/**
 * @brief operator <<
 * @param os
 * @param vid
 * @return used for writing project to file
 */
std::stringstream& operator<<(std::stringstream& os, const video& vid){
    os << vid.filepath.c_str() << " ";
    return os;
}
/**
 * @brief operator ==
 * @param v1
 * @param v2
 * @return if videos the same TRUE
 */
bool operator==(video v1, video v2){
    return v1.filepath == v2.filepath;
}
