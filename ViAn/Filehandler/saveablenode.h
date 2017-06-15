#ifndef SAVEABLENODE_H
#define SAVEABLENODE_H

// QT
#include <iostream>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
// Vian
#include "jsonnode.h"

typedef int ID;


class TypeFactory
{
    std::map<std::string, JsonNode*> m_type_map;
public:
    TypeFactory();
    JsonNode *get(const std::string & id) const;
    void set(const std::string & id, JsonNode* item);
};

class SaveableNode : public JsonNode
{
    TypeFactory factory;
protected:
    std::string m_directory = "";
    std::string m_file_name = "";
public:
    enum SAVE_FORMAT {JSON, BINARY};    // Formats supported by save_project

public:        
    SaveableNode();
    virtual ~SaveableNode();

    bool load_node(const std::string &full_path, const SAVE_FORMAT &save_format = JSON);
    bool save_node(const std::string& file_name, const std::string& dir_path, const SAVE_FORMAT& save_format = JSON);
    bool delete_node();
};

#endif // SAVEABLENODE_H
