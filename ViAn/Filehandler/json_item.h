#ifndef JSON_ITEM_H
#define JSON_ITEM_H

#include <QJsonObject>
class JsonItem
{
public:
    JsonItem();
    virtual JsonItem* clone() const = 0;
    virtual void read(const QJsonObject& json) = 0;
    virtual void write(QJsonObject& json) = 0;
    virtual ~JsonItem();
};
class TypeFactory
{
public:
    JsonItem* get(std::string const& id) const;
    void set(std::string const& id, JsonItem* item);
};

#endif // JSON_ITEM_H
