#ifndef SAVEABLENODE_H
#define SAVEABLENODE_H

// QT
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
// Vian
#include "json_item.h"

typedef int ID;
class SaveableNode
{
    TypeFactory factory;
protected:
    std::string m_directory = "";
    std::string m_file_name = "";
    std::vector<JsonItem*> m_json_children;

public:
    enum SAVE_FORMAT {JSON, BINARY};    // Formats supported by save_project
    static const SAVE_FORMAT DEFAULT_SAVE_FORMAT = JSON;

public:        
    SaveableNode();
    virtual ~SaveableNode();
    ID add_child(JsonItem* item);
    void remove_child(ID id);

    bool load_node(const std::string &full_path, const SAVE_FORMAT &save_format = DEFAULT_SAVE_FORMAT);
    bool save_node(const std::string& file_name, const std::string& dir_path, const SAVE_FORMAT& save_format = DEFAULT_SAVE_FORMAT);
    bool delete_node();
};

#endif // SAVEABLENODE_H
