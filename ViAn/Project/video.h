#ifndef VIDEO_H
#define VIDEO_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <QJsonObject>
#include <QString>
#include "Filehandler/json_item.h"
typedef int ID;
class Video : public JsonItem{

public:
    Video();
    Video(std::string file_path);
    virtual Video* clone() const { return new Video(*this); }
    virtual std::string get_type_info();
    std::string file_path;       
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    friend bool operator==(Video v1, Video v2);
};

#endif // VIDEO_H
