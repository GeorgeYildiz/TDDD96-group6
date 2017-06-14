#ifndef JSON_ITEM_H
#define JSON_ITEM_H

#include <QJsonObject>
class JsonItem
{
public:
    JsonItem();
    virtual void read(const QJsonObject& json) = 0;
    virtual void write(QJsonObject& json) = 0;
    virtual ~JsonItem();
};

#endif // JSON_ITEM_H
