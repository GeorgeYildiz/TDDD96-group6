#ifndef SAVABLE_H
#define SAVABLE_H
#include <vector>
#include "json_item.h"
#include "saveablenode.h"
// QT
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
/**
 * @brief The Saveable class
 * The saveable class is an abstract class
 * used to guarantee an objects ability to be saved
 * as a json or binary in filehandler.
 */
typedef int ID;
class SaveableTree
{    
    std::map<std::string,std::vector<SaveableNode> > m_map_saveables; // Map of directories and corresponding saveables
public:
    SaveableTree();
    // Saveable methods    
    bool save_tree();
    bool load_tree(const std::string& full_path);
    bool load_nodes();
    bool delete_tree();

    virtual ~SaveableTree();
};

#endif // SAVABLE_H
