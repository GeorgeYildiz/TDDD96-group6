#ifndef SAVEABLENODE_H
#define SAVEABLENODE_H

// QT
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
// Vian
#include "json_item.h"
class SaveableNode
{
    std::string save_name;

public:
    SaveableNode();
    virtual ~SaveableNode();

    enum SAVE_FORMAT {JSON, BINARY};    // Formats supported by save_project
    static const SAVE_FORMAT DEFAULT_SAVE_FORMAT = JSON;

    bool load_saveable(const std::string &full_path, const SAVE_FORMAT &save_format = DEFAULT_SAVE_FORMAT);
    bool save_saveable(const std::string &file_name, const std::string &dir_path, const SAVE_FORMAT &save_format = DEFAULT_SAVE_FORMAT);
    bool delete_saveable();
};

#endif // SAVEABLENODE_H
