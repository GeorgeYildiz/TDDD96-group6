#include "icononbuttonhandler.h"
#include <QMessageBox>
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief IconOnButtonHandler::IconOnButtonHandler
 * Constructor
 */

IconOnButtonHandler::IconOnButtonHandler(){
    QPixmap errorPixmap(":/error.png");
    QPixmap playPixmap(":/play.png");
    QPixmap pausePixmap(":/pause.png");
    QPixmap stopPixmap(":/stop.png");
    QPixmap nextPixmap(":/next_frame.png");
    QPixmap previousPixmap(":/previous_frame.png");

    if (errorPixmap.isNull() || playPixmap.isNull() || pausePixmap.isNull()
            || stopPixmap.isNull() || nextPixmap.isNull() || previousPixmap.isNull()){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Couldn't find one or more images!");
        messageBox.setFixedSize(500,200);
    }

    QIcon errorIcon(errorPixmap);
    iconMap["error"] = errorIcon;

    QIcon playIcon(playPixmap);
    iconMap["play"] = playIcon;

    QIcon pauseIcon(pausePixmap);
    iconMap["pause"] = pauseIcon;

    QIcon stopIcon(stopPixmap);
    iconMap["stop"] = stopIcon;

    QIcon nextIcon(nextPixmap);
    iconMap["next"] = nextIcon;

    QIcon previousIcon(previousPixmap);
    iconMap["previous"] = previousIcon;
}

/**
 * @brief IconOnButtonHandler::~IconOnButtonHandler
 * Destructor
 */
IconOnButtonHandler::~IconOnButtonHandler(){

}

/**
 * @brief IconOnButtonHandler::set_pictures_to_buttons
 * adds icons to all the buttons.
 * @param ui the window you want to add the icons to
 */
void IconOnButtonHandler::set_pictures_to_buttons(Ui::MainWindow *ui){
    ui->play_pause_button->setIcon(iconMap["play"]);
    ui->stop_button->setIcon(iconMap["stop"]);
    ui->next_frame_button->setIcon(iconMap["next"]);
    ui->previous_frame_button->setIcon(iconMap["previous"]);
}


/**
 * @brief IconOnButtonHandler::set_icon
 * if you send in a non valid number you get an error image
 * @param iconNumber
 * @param button
 */
void IconOnButtonHandler::set_icon(string icon, QPushButton *button){
    if(iconMap.find(icon) == iconMap.end()){
        button->setIcon(iconMap["error"]);
    }else{
        button->setIcon(iconMap[icon]);
    }
}
