#ifndef JSON_ITEM_H
#define JSON_ITEM_H
#include <map>
#include <QJsonObject>
class JsonItem
{
public:
    JsonItem();
    virtual JsonItem* clone() const = 0;
    virtual void read(const QJsonObject& json) = 0;
    virtual void write(QJsonObject& json) = 0;
    virtual std::string get_type_info() = 0;
    virtual ~JsonItem();
};
class TypeFactory
{
    std::map<std::string, JsonItem*> m_type_map;
public:
    TypeFactory();
    JsonItem* get(const std::string & id) const;
    void set(const std::string & id, JsonItem* item);

};

#endif // JSON_ITEM_H
