#ifndef VIDEO_H
#define VIDEO_H
#include <iostream>
#include <fstream>
#include <sstream>
typedef int ID;
class video
{
public:
    video();
    video(std::string filepath);
    std::string filepath;
    friend std::stringstream& operator>>(std::stringstream& is, video& vid);
    friend std::stringstream& operator<<(std::stringstream& os, const video& vid);
    friend bool operator==(video v1, video v2);
};

#endif // VIDEO_H
