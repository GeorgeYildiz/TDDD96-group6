#include "json_item.h"

JsonItem::JsonItem()
{

}

JsonItem::~JsonItem()
{

}

JsonItem *TypeFactory::get(const std::string &id) const
{
    return m_type_map.at(id);
}

void TypeFactory::set(const std::string &id, JsonItem *item)
{
    m_type_map.insert(std::make_pair(id,item));
}
