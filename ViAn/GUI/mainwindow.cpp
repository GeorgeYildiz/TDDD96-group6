#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include <sstream>
#include <QCloseEvent>
#include <QColorDialog>
#include <chrono>
#include <thread>
#include "icononbuttonhandler.h"
#include "Video/shapes/shape.h"

using namespace std;
using namespace cv;

/**
 * @brief MainWindow::MainWindow
 * Constructor
 * @param parent a QWidget variable
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    video_slider = ui->video_slider;


    icon_on_button_handler = new IconOnButtonHandler();
    icon_on_button_handler->set_pictures_to_buttons(ui);

    // Setup a Bookmark View in the right sidebar in the GUI.
    bookmark_view = new BookmarkView(ui->document_list);

    setup_file_handler();

    // Add this object as a listener to video_frame.
    ui->video_frame->installEventFilter(this);
    ui->video_frame->setScaledContents(false);

    ui->project_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->project_tree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::prepare_menu);

    //Creates and prepares the video_player.
    mvideo_player = new video_player(&mutex, &paused_wait, ui->video_frame);
    setup_video_player(mvideo_player);

    // Initially hide overlay and analysis toolbar
    ui->toolbar_overlay->hide();
    ui->toolbar_analysis->hide();

    // The video player is not in original size.
    original_size = false;
}


/**
 * @brief MainWindow::~MainWindow
 * Destructor
 */
MainWindow::~MainWindow() {

    delete icon_on_button_handler;
    delete file_handler;

    if (mvideo_player->is_paused())
        paused_wait.wakeOne();

    emit set_stop_video(); //Signaling the run function in mvideo_player to stop executing
    mvideo_player->wait(); //Waiting for video_player to finish executing
    delete mvideo_player;

    delete ui;
    delete bookmark_view;
}

/**
 * @brief MainWindow::set_status_bar
 * @param status text to show in the status_bar
 * @param timer time to show it in the bar in ms, 750ms is standard
 */
void MainWindow::set_status_bar(std::string status, int timer){
    ui->status_bar->showMessage(QString::fromStdString(status), timer);
}

/**
 * @brief MainWindow::resizeEvent
 * Used for rescaling the source image for video playback
 * @param event
 */
void MainWindow::resizeEvent(QResizeEvent* event) {
   QMainWindow::resizeEvent(event);

   on_action_fill_screen_triggered();
}

/**
 * @brief MainWindow::closeEvent
 * asks if you are sure you want to quit.
 * TODO Needs to close all other threads before exiting the program
 * @param event closing
 */
void MainWindow::closeEvent (QCloseEvent *event) {
    set_status_bar("Closing");
    QMessageBox::StandardButton res_btn = QMessageBox::question( this, "Exit",
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);

    if (res_btn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
}

/**
 * @brief MainWindow::on_action_exit_triggered
 * sends a closeEvent when you press exit
 */
void MainWindow::on_action_exit_triggered() {
    this->close();
}

/**
 * @brief MainWindow::on_bookmark_button_clicked
 * Button to add a bookmark to the bookmark view.
 */
void MainWindow::on_bookmark_button_clicked() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        // Get current project.
        item = ui->project_tree->selectedItems().first();
        my_project = (MyQTreeWidgetItem*)get_project_from_object(item);
        // Add bookmarks-folder to the project-folder.
        Project* proj = file_handler->get_project(my_project->id);
        QDir dir = file_handler->get_dir(proj->dir_bookmarks);
        // Export the current frame in the bookmarks-folder.
        // Get bookmark description
        QString bookmark_text("");
        bool ok;
        bookmark_text = bookmark_view->get_input_text(&ok);
        if(!ok) return;
        int frame_number = mvideo_player->get_current_frame_num();
        QImage frame = mvideo_player->get_current_frame_unscaled();
        Bookmark* bookmark = new Bookmark(frame_number, frame, dir.absolutePath(), bookmark_text);
        proj->add_bookmark(((MyQTreeWidgetItem*)item)->id, bookmark);
        bookmark_view->add_bookmark(bookmark);
        set_status_bar("Bookmark created.");
    }
}

/**
 * @brief MainWindow::on_project_tree_itemDoubleClicked
 * @param item, the item in the project_tree that was clicked
 * @param column, the column in the tree
 * Double clicking on a video will start to play it.
 * Double clicking on a project will expand or collapse it.
 */
void MainWindow::on_project_tree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    MyQTreeWidgetItem *q_item = (MyQTreeWidgetItem*)item;
    switch(q_item->type) {
    case TYPE::PROJECT:
        if (item->isExpanded()) {
            item->setExpanded(true);
        } else {
            item->setExpanded(false);
        }
        break;
    case TYPE::VIDEO:
        play_video();
        break;
    default:
        break;
    }
}

/**
 * @brief MainWindow::eventFilter
 * Listener function for all eventFilters MainWindow has installed.
 * @param obj the object invoking the event
 * @param event the invooked event
 * @return Returns true if the event matched any of the overlay's
 *         functionality, else false.
 *         (Returning false means that the event is sent to the
 *          target object instead, but not if true is returned.)
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    // Check who invoked the event.
    if (qobject_cast<QLabel*>(obj)==ui->video_frame) {
        // Cast to a mouse event to get the mouse position.
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        // Check what kind of event.
        if (event->type() == QEvent::MouseButtonPress) {
            mvideo_player->video_mouse_pressed(pos);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            mvideo_player->video_mouse_released(pos);
            return true;
        } else if (event->type() == QEvent::MouseMove) {
            mvideo_player->video_mouse_moved(pos);
            return true;
        }
    }
    return false;
}

/**
 * @brief MainWindow::prepare_menu
 * @param pos
 * Creates context menu on right-click in tree view
 */
void MainWindow::prepare_menu(const QPoint & pos) {
    QTreeWidget *tree = ui->project_tree;
    MyQTreeWidgetItem *item = (MyQTreeWidgetItem*)tree->itemAt( pos );
    QMenu menu(this);

    QAction *create_project = new QAction(QIcon(""), tr("&Create project"), this);
    QAction *load_project = new QAction(QIcon(""), tr("&Load project"), this);
    create_project->setStatusTip(tr("Create project"));
    load_project->setStatusTip(tr("Load project"));
    menu.addAction(create_project);
    menu.addAction(load_project);
    connect(create_project, SIGNAL(triggered()), this, SLOT(on_action_add_project_triggered()));
    connect(load_project, SIGNAL(triggered()), this, SLOT(on_action_load_triggered()));
    if(item != nullptr) {
        if(item->type == TYPE::PROJECT) {
            QAction *add_video = new QAction(QIcon(""), tr("&Add video"), this);
            QAction *delete_project = new QAction(QIcon(""), tr("&Delete project"), this);
            add_video->setStatusTip(tr("Add video"));
            delete_project->setStatusTip(tr("Delete project"));
            menu.addAction(add_video);
            menu.addAction(delete_project);
            connect(add_video, SIGNAL(triggered()), this, SLOT(on_action_add_video_triggered()));
            connect(delete_project, SIGNAL(triggered()), this, SLOT(on_action_delete_triggered()));
        } else if(item->type == TYPE::VIDEO) {
            QAction *load_video = new QAction(QIcon(""), tr("&Play video"), this);
            QAction *delete_video = new QAction(QIcon(""), tr("&Remove video"), this);
            load_video->setStatusTip(tr("Play video"));
            delete_video->setStatusTip(tr("Remove video from project"));
            menu.addAction(load_video);
            menu.addAction(delete_video);
            connect(load_video, SIGNAL(triggered()), this, SLOT(play_video()));
            connect(delete_video, SIGNAL(triggered()), this, SLOT(on_action_delete_triggered()));
        }
        QAction *close_project = new QAction(QIcon(""), tr("&Close project"), this);
        close_project->setStatusTip(tr("Close project"));
        menu.addAction(close_project);
        connect(close_project, SIGNAL(triggered()), this, SLOT(on_action_close_project_triggered()));
    }
    QPoint pt(pos);
    menu.exec( tree->mapToGlobal(pos) );
}

/**
 * @brief MainWindow::on_action_delete_triggered
 * Deletes the selected item in the tree
 */
void MainWindow::on_action_delete_triggered() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        item = ui->project_tree->selectedItems().first();
        my_item = (MyQTreeWidgetItem*)item;
        QMessageBox::StandardButton res_btn = QMessageBox::question( this, "Delete",
                                                                    tr(("Are you sure you want to delete " + my_item->get_name() + "?\n").c_str()),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);

        if (res_btn == QMessageBox::Yes) {
            if (my_item->type == TYPE::VIDEO) {
                my_project = (MyQTreeWidgetItem*) get_project_from_object(item);
                this->file_handler->remove_video_from_project(my_project->id, my_item->id); // Remove video from project
            } else if (my_item->type == TYPE::PROJECT) {
                this->file_handler->delete_project(my_item->id);
            }
            remove_item_from_tree(my_item);
            set_status_bar("Remove item");
        }
    } else {
        set_status_bar("Multiple or no videos selected.");
    }
}

/**
 * @brief MainWindow::remove_item_from_tree
 * @param my_item item to be removed from tree
 */
void MainWindow::remove_item_from_tree(MyQTreeWidgetItem *my_item) {
    delete my_item;
}

/**
 * @brief MainWindow::toggle_toolbar
 * This method will toggle the toolbar depending on whether the overlay and analysis is showing or not.
 * It is switching between a toolbar that contains items as save/add/load and another that
 * contains drawing tools, and a third containing analysis tools.
 * This is invoked when the overlay and analysis tool is activated and deactivated.
 */
void MainWindow::toggle_toolbar() {
    if (mvideo_player->is_showing_analysis_tool()) {
        ui->toolbar_analysis->show();
        ui->toolbar->hide();
        ui->toolbar_overlay->hide();
        ui->action_show_hide_overlay->setEnabled(false);
    } else if (mvideo_player->is_showing_overlay()) {
        ui->toolbar_analysis->hide();
        ui->toolbar->hide();
        ui->toolbar_overlay->show();
        ui->action_show_hide_analysis_area->setEnabled(false);
    } else {
        ui->toolbar_analysis->hide();
        ui->toolbar->show();
        ui->toolbar_overlay->hide();
        ui->action_show_hide_overlay->setEnabled(true);
        ui->action_show_hide_analysis_area->setEnabled(true);
    }
}

/**
 * @brief MainWindow::enable_video_buttons
 * Enable the videobuttons.
 * They are disabled as default.
 */
void MainWindow::enable_video_buttons() {
    ui->next_frame_button->setEnabled(true);
    ui->decrease_speed_button->setEnabled(true);
    ui->play_pause_button->setEnabled(true);
    ui->increase_speed_button->setEnabled(true);
    ui->previous_frame_button->setEnabled(true);
    ui->stop_button->setEnabled(true);
    ui->bookmark_button->setEnabled(true);
    ui->previous_POI_button->setEnabled(true);
    ui->next_POI_button->setEnabled(true);
    ui->video_slider->setEnabled(true);
}

/**
 * @brief MainWindow::get_project_from_object
 * @param item item you want to get the topmost parent of.
 * @return
 */
QTreeWidgetItem *MainWindow::get_project_from_object(QTreeWidgetItem* item) {
    QTreeWidgetItem *project = item;
    while (project->parent()){
        project = project->parent();
    }
    return project;
}

/**
 * @brief MainWindow::on_action_fill_screen_triggered
 * Reseizes the video to fit the size of the screen.
 * Will not work if you have set it to original size.
 */
void MainWindow::on_action_fill_screen_triggered() {
    if(!original_size) {
        //Sends new scroll area resolution to mvideo_player to update scaling resolution
        // Video frame is in the scroll area
        emit resize_video_frame((ui->frame_scroll_area->width())-SCROLL_AREA_MARGIN, (ui->frame_scroll_area->height())-SCROLL_AREA_MARGIN);
    }
}

/**
 * @brief MainWindow::on_document_list_itemClicked
 * Invoked when an item in the bookmark view has been clicked.
 * @param item The bookmark that has been clicked.
 */
void MainWindow::on_document_list_itemClicked(QListWidgetItem *item) {
    BookmarkItem* bookmark = (BookmarkItem*) item;
    emit set_playback_frame(bookmark->get_frame_number());
    set_status_bar("Jump to frame: " + to_string(bookmark->get_frame_number()) + ".");
}
