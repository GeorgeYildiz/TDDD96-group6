#include "filehandlertest.h"
/**
 * @brief runTestSuite
 * @return all pass errorcode,
 * outputs information about which tests passed if
 * PRINTS set in header to terminal
 *
 */
//void filehandler_testsuite::runTestSuite(){
//    FileHandler* fileHandler = new FileHandler();
//    std::string dirpath = WORKSPACE;
//    Project* proj  = fileHandler->createProject("POI_PROJ");
//    directoryTest(fileHandler, proj);
//    fileTest(fileHandler, proj);
//    projectHandlingTest(fileHandler, proj);
//}

//
// Tests that creation, manipulation and deletion of
// projects functions as intended
//
filehandlertest::filehandlertest(QObject *parent) : QObject(parent){

}
/**
 * @brief FileHandlerTest::projectHandlingTest
 * not much to test currently, will be extended
 * with furter implementation of project manipulation
 */
void filehandlertest::projectHandlingTest(){
    FileHandler* fh = new FileHandler();
    Project* proj =  fh->createProject("/TEST_PROJ");
    std::string v1 = "video1.txt"; // names for testfiles
    std::string v2 = "video2.txt";
    std::string v3 = "video3.txt";
    ID vid1 = fh->createFile(v1, proj->m_dir);  // create testfiles
    ID vid2 = fh->createFile(v2, proj->m_dir);
    ID vid3 = fh->createFile(v3, proj->m_dir);

    fh->addVideo(proj, fh->getFile(vid1)); //add tesfiles as videos to project.
    fh->addVideo(proj, fh->getFile(vid2));
    fh->addVideo(proj, fh->getFile(vid3));
    fh->saveProject(proj);
    //check file contents
    fh->loadProject(fh->getDir(proj->m_dir));
    //check project contentss
    fh->deleteFile(vid1);
    fh->deleteFile(vid2);
    fh->deleteFile(vid3);
    fh->deleteProject(proj);
}
//
// Test that creation and deletion of directories are working correctly.
//
void filehandlertest::directoryTest(){
    FileHandler* fh = new FileHandler();
    std::string dirpath = std::string(WORKSPACE) + std::string("/TEST_PROJ");
    int err = fh->createDirectory(dirpath);
    QCOMPARE(err, 0);
    QCOMPARE(fh->deleteDirectory(dirpath), 0);
}
// fileTest tests following functions, see filehandler.h
//
// ID createDirectory(std::string path);
// int deleteDirectory(std::string dirpath);
// ID createFile(std::string filename, ID dirID);
// int deleteFile(ID id);
// void writeFile(ID id, std::string text);

void filehandlertest::fileTest(){
    FileHandler* fh = new FileHandler();
    std::string dirpath = std::string(WORKSPACE) + "/TEST_MAP";
    std::string filename = "filetest.txt";
    ID dir = fh->createDirectory(dirpath);
    ID fileID  = fh->createFile(filename,dir); //Create file ID = i    (1)
    ID fileID2  = fh->createFile(filename,dir); // Create file ID = i+1 (2)
    QCOMPARE(fileID2 - fileID, 1);                         // check if 1 && 2 is true
    QVERIFY(fh->getFile(fileID) == fh->getFile(fileID2)); //check that (1), (2) point to same file.

    // TEST FILE WRITE AND FILE READ
    fh->writeFile(fileID, filename);   //write to file "filetest.txt"
    //read and check if file contains filename"
    std::string readText;
    fh->readFile(fileID,1, readText); // read from file
    QCOMPARE(readText, filename); // d = 0 if test passes
    //delete file and check if it is not there
    QCOMPARE(fh->deleteFile(fileID), 0); // check that delete was done correctly
    QCOMPARE(fh->deleteDirectory(dirpath), 0); //delete directory

}
