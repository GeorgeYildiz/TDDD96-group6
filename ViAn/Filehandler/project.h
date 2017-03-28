#ifndef PROJECT_H
#define PROJECT_H
#include <map>
#include <string>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include "filehandler.h"
#include "video.h"
typedef int ID;
/**
 * @brief The ProjectStream struct
 * Used for writing projects to and from files.
 * Importand for seperation of different types of data.
 */
struct ProjectStream{
    std::stringstream projfile;
    std::stringstream videos;
    std::stringstream analyzes;
    std::stringstream drawings;
};
/**
 * @brief The ProjFiles struct
 * project file container, simplifies code for readability,
 * also easier to pass all files as opposed to every file
 * seperately
 */
struct proj_files{
   ID dir;
   ID f_proj;
   ID f_analysis;
   ID f_drawings;
   ID f_videos;
   proj_files(){
       this->dir = -1;
       this->f_proj = -1;
       this->f_analysis = -1;
       this->f_drawings = -1;
       this->f_videos = -1;
   }

   friend ProjectStream& operator>>(ProjectStream &ps, proj_files& pf);
   friend ProjectStream& operator<<(ProjectStream &ps,const proj_files& pf);
};

/**
 * @brief The Project struct
 * incomplete struct, will be added on
 * along with parser functionality
 */

struct project{
public:
    project();
    project(ID id, std::string name);
    project(std::string dirpath);
    void add_video(video *vid);

    // read and write operator for Projects
    friend ProjectStream& operator>>(ProjectStream& ps, project& proj);
    friend ProjectStream& operator<<(ProjectStream& ps, const project& proj);



// TODO
//    void add_analysis();
//    void add_drawing();      
public:
    ID id;
    std::string name;
    proj_files* files;
    std::vector<video*> videos;
    bool saved;
};


#endif // PROJECT_H
