#ifndef REPORT_H
#define REPORT_H
#include <string>
#include "Filehandler/jsonnode.h"
class Report : public JsonNode{
    std::string file_path;
public:
    Report(std::string file_path);
    Report();
    virtual std::string get_type_info();
    virtual Report* clone() const { return new Report(*this); }
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    std::string get_file_path() const;
};

#endif // REPORT_H
