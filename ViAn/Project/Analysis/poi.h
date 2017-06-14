#ifndef POI_H
#define POI_H
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QObject>
#include <vector>
#include <map>
#include "Filehandler/json_item.h"
#include "opencv2/core/core.hpp"
#include "ooi.h"
class POI : public JsonItem{
    std::map<int,std::vector<OOI>> OOIs;
public:
    POI();

    virtual POI* clone() const { return new POI(*this); }

    int start_frame = -1;
    int end_frame = -1;
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    void add_detections(int frame_num, std::vector<OOI> detections);
    void set_end_frame(int frame_num);
    friend class Analysis;
};


#endif // POI_H
