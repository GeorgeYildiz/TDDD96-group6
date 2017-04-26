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
#include "inputwindow.h"
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

    icon_on_button_handler = new IconOnButtonHandler();
    icon_on_button_handler->set_pictures_to_buttons(ui);

    // Setup a Bookmark View in the right sidebar in the GUI.
    bookmark_view = new BookmarkView(ui->document_list);

    fileHandler = new FileHandler();

    // Add this object as a listener to video_frame.
    ui->video_frame->installEventFilter(this);
    ui->video_frame->setScaledContents(false);

    ui->project_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->project_tree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::prepare_menu);

    //Creates and prepares the video_player.
    mvideo_player = new video_player(&mutex, &paused_wait, ui->video_frame);
    setup_video_player(mvideo_player);

    // Initially hide overlay and analysis toolbar
    ui->toolBar_overlay->hide();
    ui->toolBar_analysis->hide();

    // The video player is not in original size.
    original_size = false;
}


/**
 * @brief MainWindow::~MainWindow
 * Destructor
 */
MainWindow::~MainWindow() {
    delete icon_on_button_handler;
    delete fileHandler;

    if (mvideo_player->is_paused())
        paused_wait.wakeOne();

    emit set_stop_video(); //Signaling the run function in mvideo_player to stop executing
    mvideo_player->wait(); //Waiting for video_player to finish executing
    delete mvideo_player;

    delete ui;
    delete bookmark_view;
}

/**
 * @brief MainWindow::setup_video_player
 * @param mplayer
 * Connects all signals and slots that are needed between video_player and mainwindow.
 */
void MainWindow::setup_video_player(video_player *mplayer) {
    QObject::connect(mplayer, SIGNAL(processed_image(QImage)),
                     this, SLOT(update_video(QImage)));
    QObject::connect(mplayer, SIGNAL(update_current_frame(int)),
                     this, SLOT(set_video_slider_pos(int)));
    QObject::connect(this, SIGNAL(resize_video_frame(int,int)),
                     mplayer, SLOT(scaling_event(int,int)));
    QObject::connect(this, SIGNAL(next_video_frame()),
                     mplayer, SLOT(next_frame()));
    QObject::connect(this, SIGNAL(prev_video_frame()),
                     mplayer, SLOT(previous_frame()));
    QObject::connect(this, SIGNAL(set_play_video()),
                     mplayer, SLOT(on_play_video()));
    QObject::connect(this, SIGNAL(set_pause_video()),
                     mplayer, SLOT(on_pause_video()));
    QObject::connect(this, SIGNAL(set_stop_video()),
                     mplayer, SLOT(on_stop_video()));
    QObject::connect(this, SIGNAL(set_playback_frame(int)),
                     mplayer, SLOT(on_set_playback_frame(int)));
    QObject::connect(mplayer, SIGNAL(finished()),
                     mplayer, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(set_abort_video()),
                     mplayer, SLOT(on_abort_video()));
}

/**
 * @brief MainWindow::set_status_bar
 * @param status text to show in the statusbar
 * @param timer time to show it in the bar in ms, 750ms is standard
 */
void MainWindow::set_status_bar(std::string status, int timer){
    ui->statusBar->showMessage(QString::fromStdString(status), timer);
}

/**
 * @brief MainWindow::on_decrease_speed_button_clicked
 * Calls upon a video player function which decreases the playback speed
 *
 */
void MainWindow::on_decrease_speed_button_clicked(){
    mvideo_player->dec_playback_speed();
    double curr_speed_factor = 1/mvideo_player->get_speed_multiplier();
    std::ostringstream speed_str;
    speed_str << curr_speed_factor;
    set_status_bar("Playback speed decreased (x" + speed_str.str() + ")");
}

/**
 * @brief MainWindow::on_play_pause_button_clicked
 * Calls upon video player functions based on current playback status
 * Starts/resumes a stopped/paused video, pauses a playing one
 */
void MainWindow::on_play_pause_button_clicked() {
    if (mvideo_player->is_paused()) {
        // Video thread is paused. Notifying the waitcondition to resume playback
        icon_on_button_handler->set_icon("pause", ui->play_pause_button);//changes the icon on the play button to a pause-icon
        paused_wait.wakeOne();
        set_status_bar("Playing");
    } else if (mvideo_player->is_stopped()) {
        // Video thread has finished. Start a new one
        icon_on_button_handler->set_icon("pause", ui->play_pause_button);
        mvideo_player->start();
        set_status_bar("Playing");
    } else {
        // Video thread is running. Pause it
        icon_on_button_handler->set_icon("play", ui->play_pause_button);
        emit set_pause_video();
        set_status_bar("Paused");
    }
}

/**
 * @brief MainWindow::on_increase_speed_button_clicked
 * Calls upon video player function which in turn increases the playback speed
 */
void MainWindow::on_increase_speed_button_clicked(){
    mvideo_player->inc_playback_speed();
    double curr_speed_factor = 1/mvideo_player->get_speed_multiplier();
    std::ostringstream speed_str;
    speed_str << curr_speed_factor;
    set_status_bar("Playback speed increased (x" + speed_str.str() + ")");
}

/**
 * @brief MainWindow::on_stop_button_clicked
 * Calls upon video player function which in turn stops the video
 */
void MainWindow::on_stop_button_clicked() {
    set_status_bar("Stopped");
    if (!mvideo_player->is_paused()) {
        icon_on_button_handler->set_icon("play", ui->play_pause_button);
    } else if (mvideo_player->is_stopped()){
        return;
    } else {
        paused_wait.wakeOne();
    }
    emit set_stop_video();
}

/**
 * @brief MainWindow::on_next_frame_button_clicked
 * Calls upon video player function which in turn skips to the next frame
 */
void MainWindow::on_next_frame_button_clicked() {
    if (mvideo_player->is_paused()) {
        set_status_bar("Went forward a frame");
        emit next_video_frame();
    } else {
        set_status_bar("Needs to be paused");
    }
}

/**
 * @brief MainWindow::on_previous_frame_button_clicked
 * Calls upon video player function which in turn steps back to the previous frame
 */
void MainWindow::on_previous_frame_button_clicked() {
    if (mvideo_player->is_paused()) {
        emit prev_video_frame();
        set_status_bar("Went back a frame");
    } else {
        set_status_bar("Video needs to be paused");
    }
}

/**
 * @brief MainWindow::update_video
 * Sets the video_frame pixmap to the current frame from video
 * @param frame
 */
void MainWindow::update_video(QImage frame) {
    ui->video_frame->setPixmap(QPixmap::fromImage(frame));
}

/**
 * @brief MainWindow::set_video_slider_pos
 * Sets the position of slider in video to position pos
 * @param pos
 */
void MainWindow::set_video_slider_pos(int pos) {
    if (pos <= video_slider->maximum() && !slider_blocked) {
        prev_slider_pos = video_slider->value();
        video_slider->setSliderPosition(pos);
    }
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
 * @brief MainWindow::on_slider_moving
 * Sends frame numbers to the video player whilst the slider is moving
 */
void MainWindow::on_slider_moving(){
    //This code makes sure that the currently shown frame is not updated more often than
    //every SLIDER_UPDATE_TIMER milliseconds for performance reasons.
    std::chrono::milliseconds current_time = std::chrono::duration_cast<
            std::chrono::milliseconds >(
                std::chrono::system_clock::now().time_since_epoch()
    );
    std::chrono::milliseconds time_since_last_slider_frame_update = current_time-slider_timer;
    if (time_since_last_slider_frame_update.count() >= SLIDER_UPDATE_TIMER) {
        QPoint local_mouse_pos = ui->video_slider->mapFromGlobal(QCursor::pos());
        emit set_playback_frame(slider_pos_under_mouse(local_mouse_pos));
        slider_timer = current_time;
    }
}

/**
 * @brief MainWindow::on_slider_click
 * Enables the slider marker to be moved to the position where a click occured
 * It also sends the clicked frame value to the video player
 * @param new_pos
 * @param local_mouse_pos
 */
void MainWindow::on_slider_click(int new_pos, QPoint local_mouse_pos){
    int slider_pos = slider_pos_under_mouse(local_mouse_pos);
    if (slider_pos != new_pos) {
        ui->video_slider->setValue(slider_pos);
        emit set_playback_frame(slider_pos, true);
    }
}

/**
 * @brief MainWindow::slider_pos_under_mouse
 * Takes the general position of the mouse pointer and calculates which
 * position on the slider it relates to.
 * @param local_mouse_pos is the general position of the mouse pointer
 * @return The current position of the mouse pointer on the slider
 */
int MainWindow::slider_pos_under_mouse(QPoint local_mouse_pos) {
    float pos_ratio = local_mouse_pos.x() / (float )ui->video_slider->size().width();
    int slider_range = ui->video_slider->maximum() - ui->video_slider->minimum();
    return ui->video_slider->minimum() + slider_range * pos_ratio;
}

/**
 * @brief MainWindow::on_video_slider_valueChanged
 * Gets called when the value of the slider is changed
 * Moves the slider marker and sends the new frame number to the video player
 * @param new_pos current position of the slider
 */

void MainWindow::on_video_slider_valueChanged(int new_pos) {
    slider_blocked = true;
    Qt::MouseButtons btns = QApplication::mouseButtons();
    QPoint local_mouse_pos = ui->video_slider->mapFromGlobal(QCursor::pos());
    bool click_on_slider = (btns & Qt::LeftButton) &&
                         (local_mouse_pos.x() >= 0 && local_mouse_pos.y() >= 0 &&
                          local_mouse_pos.x() < ui->video_slider->size().width() &&
                          local_mouse_pos.y() < ui->video_slider->size().height());
    if (click_on_slider) on_slider_click(new_pos, local_mouse_pos);
    slider_blocked = false;
}

/**
 * @brief MainWindow::closeEvent
 * asks if you are sure you want to quit.
 * TODO Needs to close all other threads before exiting the program
 * @param event closing
 */
void MainWindow::closeEvent (QCloseEvent *event) {
    set_status_bar("Closing");
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);

    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
}

/**
 * @brief MainWindow::on_actionExit_triggered
 * sends a closeEvent when you press exit
 */
void MainWindow::on_actionExit_triggered() {
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
        Project* proj = fileHandler->get_project(my_project->id);
        QDir dir = fileHandler->get_dir(proj->bookmark_dir);
        // Export the current frame in the bookmarks-folder.
        // Get bookmark description
        QString bookmark_text("");
        bool ok;
        bookmark_text = bookmark_view->get_input_text(&ok);
        if(!ok) return;
        // The names of the stored files will have increasing numbers.
        std::string file_name = std::to_string(bookmark_view->get_num_bookmarks());
        /*
        std::string file_path = mvideo_player->export_current_frame(dir_path, file_name);

        bookmark_view->add_bookmark(mvideo_player->get_video_name() , mvideo_player->get_current_frame_num()+1, file_path);
        */
        std::string file_path = mvideo_player->export_current_frame(dir.absolutePath().toStdString(), file_name);
        int frame = mvideo_player->get_current_frame_num();
        Bookmark* bookmark = new Bookmark(frame ,QString::fromStdString(file_path), bookmark_text);
        proj->add_bookmark(((MyQTreeWidgetItem*)item)->id, bookmark);
        bookmark_view->add_bookmark(bookmark);

        set_status_bar("Saved bookmark.");
    }
}

/**
 * @brief MainWindow::on_actionAddProject_triggered
 */
void MainWindow::on_actionAddProject_triggered() {
    ACTION action = ADD_PROJECT;
    input_window = new inputwindow(this, action, "Project name:");
    input_window->show();
    set_status_bar("Adding project, need name");
}

/**
 * @brief MainWindow::inputSwitchCase
 * @param input the input from the user
 * @param action the action that was triggered earlier
 */
void MainWindow::input_switch_case(ACTION action, QString qInput) {    
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        my_project = (MyQTreeWidgetItem*)ui->project_tree->selectedItems().first();
    } else {
        set_status_bar("Multiple or no things selected.");
    }
    switch(action) {
    case ADD_PROJECT: {
        Project* proj = fileHandler->create_project(qInput);
        add_project_to_tree(proj);
        set_status_bar("Project " + qInput.toStdString() + " created.");
        delete input_window;
        break;
    }
    case CANCEL:
        set_status_bar("Cancel");
        delete input_window;
        break;
    case ADD_VIDEO: {
        ID id = fileHandler->add_video(fileHandler->get_project(my_project->id), qInput.toStdString());
        add_video_to_tree(qInput.toStdString(), id);
        set_status_bar("Video " + qInput.toStdString() + " added.");
        break;
    }
    default:
        break;
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

 /** @brief MainWindow::on_actionShow_hide_overlay_triggered
 * Toggles the showing/hiding of the overlay.
 * Invoked by menu item.
 */
void MainWindow::on_actionShow_hide_overlay_triggered() {
    mvideo_player->toggle_overlay();
    toggle_toolbar();
    if (mvideo_player->is_showing_overlay()) {
        set_status_bar("Overlay: On.");
    } else {
        set_status_bar("Overlay: Off.");
    }
}

/**
 * @brief MainWindow::on_actionColour_triggered
 * Selects a colour for the overlay drawing tool.
 */
void MainWindow::on_actionColour_triggered() {
    QColor col = QColorDialog::getColor();
    if (col.isValid()) {
        mvideo_player->set_overlay_colour(col);
        std::string msg = "Color: ";
        msg.append(col.name().toStdString());
        set_status_bar(msg);
    }
}

/**
 * @brief MainWindow::on_actionRectangle_triggered
 * Selects the rectangle shape for the overlay drawing tool.
 */
void MainWindow::on_actionRectangle_triggered() {
    mvideo_player->set_overlay_tool(RECTANGLE);
    set_status_bar("Tool: rectangle.");
}

/**
 * @brief MainWindow::on_actionCircle_triggered
 * Selects the circle shape for the overlay drawing tool.
 */
void MainWindow::on_actionCircle_triggered() {
    mvideo_player->set_overlay_tool(CIRCLE);
    set_status_bar("Tool: circle.");
}

/**
 * @brief MainWindow::on_actionLine_triggered
 * Selects the line shape for the overlay drawing tool.
 */
void MainWindow::on_actionLine_triggered() {
    mvideo_player->set_overlay_tool(LINE);
    set_status_bar("Tool: line.");
}

/**
 * @brief MainWindow::on_actionArrow_triggered
 * Selects the arrow shape for the overlay drawing tool.
 */
void MainWindow::on_actionArrow_triggered() {
    mvideo_player->set_overlay_tool(ARROW);
    set_status_bar("Tool: arrow.");
}

/**
 * @brief MainWindow::on_actionPen_triggered
 * Selects the pen for the overlay drawing tool.
 */
void MainWindow::on_actionPen_triggered() {
    mvideo_player->set_overlay_tool(PEN);
    set_status_bar("Tool: pen.");
}

/**
 * @brief MainWindow::on_actionText_triggered
 * Selects the text for the overlay drawing tool.
 */
void MainWindow::on_actionText_triggered() {
    mvideo_player->set_overlay_tool(TEXT);
    set_status_bar("Tool: text.");
}

/**
 * @brief MainWindow::on_actionUndo_triggered
 * Undo the drawings on the overlay.
 */
void MainWindow::on_actionUndo_triggered() {
    mvideo_player->undo_overlay();
    set_status_bar("Undo drawing.");
}

/**
 * @brief MainWindow::on_actionClear_triggered
 * Clear the drawings on the overlay.
 */
void MainWindow::on_actionClear_triggered() {
    mvideo_player->clear_overlay();
    set_status_bar("Cleared drawings.");
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
 * @brief MainWindow::on_actionZoom_in_triggered
 * Sets a state in the video overlay
 * for the user to choose an area.
 */
void MainWindow::on_actionZoom_in_triggered() {
    mvideo_player->zoom_in();
    set_status_bar("Zoom in. Choose your area.");
}

/**
 * @brief MainWindow::on_actionZoom_out_triggered
 * Reset the zoom level to the full video size.
 */
void MainWindow::on_actionZoom_out_triggered() {
    mvideo_player->zoom_out();
    set_status_bar("Zoom out.");
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
    connect(create_project, SIGNAL(triggered()), this, SLOT(on_actionAddProject_triggered()));
    connect(load_project, SIGNAL(triggered()), this, SLOT(on_actionLoad_triggered()));
    if(item != nullptr) {
        if(item->type == TYPE::PROJECT) {
            QAction *add_video = new QAction(QIcon(""), tr("&Add video"), this);
            QAction *delete_project = new QAction(QIcon(""), tr("&Delete project"), this);
            add_video->setStatusTip(tr("Add video"));
            delete_project->setStatusTip(tr("Delete project"));
            menu.addAction(add_video);
            menu.addAction(delete_project);
            connect(add_video, SIGNAL(triggered()), this, SLOT(on_actionAddVideo_triggered()));
            connect(delete_project, SIGNAL(triggered()), this, SLOT(on_actionDelete_triggered()));
        } else if(item->type == TYPE::VIDEO) {
            QAction *load_video = new QAction(QIcon(""), tr("&Play video"), this);
            QAction *delete_video = new QAction(QIcon(""), tr("&Remove video"), this);
            load_video->setStatusTip(tr("Play video"));
            delete_video->setStatusTip(tr("Remove video from project"));
            menu.addAction(load_video);
            menu.addAction(delete_video);
            connect(load_video, SIGNAL(triggered()), this, SLOT(play_video()));
            connect(delete_video, SIGNAL(triggered()), this, SLOT(on_actionDelete_triggered()));
        }
    }
    QPoint pt(pos);
    menu.exec( tree->mapToGlobal(pos) );
}

/**
 * @brief MainWindow::on_actionAddVideo_triggered
 * Prompts user with file browser to add video
 * to selected project
 */
void MainWindow::on_actionAddVideo_triggered() {
    QTreeWidgetItem *project;
    if(ui->project_tree->selectedItems().size() == 1) {
        project = ui->project_tree->selectedItems().first();
        if (((MyQTreeWidgetItem*)project)->type == TYPE::PROJECT){
            QString dir = QFileDialog::getOpenFileName(this, tr("Choose video"), this->fileHandler->get_work_space().absolutePath().toStdString().c_str(),
                                                       tr("Videos (*.avi *.mkv *.mov *.mp4 *.3gp *.flv *.webm *.ogv *.m4v)"));
            if(!dir.isEmpty()) { // Check if you have selected something.
                input_switch_case(ACTION::ADD_VIDEO, dir);
            }
        } else {
            set_status_bar("No project selected.");
        }
    } else {
        set_status_bar("Multiple or no projects selected.");
    }
}

/**
 * @brief MainWindow::play_video
 *  Loads selected video, flips playbutton to pause
 *  plays video from beginning
 */
void MainWindow::play_video() {
    MyQTreeWidgetItem *my_project;
    my_project = (MyQTreeWidgetItem*)ui->ProjectTree->selectedItems().first();
    load_new_video(my_project->name.toStdString());
}

/**
 * @brief MainWindow::load_new_video
 * Closes the currently shown video if necessary, rescales the video frame size
 * and starts playing the video specified by video_name parameter.
 * @param video_name
 */
void MainWindow::load_new_video(std::string video_name, int start_frame, bool start_paused) {
    if (mvideo_player->is_paused())
        paused_wait.wakeOne();

    if (mvideo_player->isRunning()) {
        emit set_abort_video(); //This signal will make the QThread finish executing
        mvideo_player->wait();
        delete mvideo_player;
        mvideo_player = new video_player(&mutex, &paused_wait, ui->video_frame, start_paused);
        setup_video_player(mvideo_player);
        set_status_bar("Switch playback to video: " + video_name);
    }
    mvideo_player->load_video(video_name, start_frame);

    //Used for rescaling the source image for video playback
    emit resize_video_frame(ui->video_frame->width(),ui->video_frame->height());
    enable_video_buttons();

    if (mvideo_player->is_paused())
        iconOnButtonHandler->set_icon("play", ui->playPauseButton);
    else
        iconOnButtonHandler->set_icon("pause", ui->playPauseButton);

    video_slider->setMaximum(mvideo_player->get_num_frames() - 1);
}

/**
 * @brief MainWindow::on_actionSave_triggered
 * saves project which is selected in tree view,
 * checks if there is one
 */
void MainWindow::on_actionSave_triggered() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        item = ui->project_tree->selectedItems().first();
        my_project = (MyQTreeWidgetItem*)get_project_from_object(item);
        this->fileHandler->save_project(my_project->id);
        std::string text = "Saved project " + my_project->name.toStdString();
        set_status_bar(text);
    } else {
        set_status_bar("Nothing to save");
    }
}

/**
 * @brief MainWindow::on_actionLoad_triggered
 */
void MainWindow::on_actionLoad_triggered() {
    QString dir = QFileDialog::getOpenFileName(this, tr("Choose project"),this->fileHandler->get_work_space().absolutePath().toStdString().c_str(),tr("*.json"));
    if(!dir.isEmpty()) { // Check if you have selected something.
        Project* loadProj= this->fileHandler->load_project(dir.toStdString());
        add_project_to_tree(loadProj);
        set_status_bar("Project " + loadProj->name + " loaded.");
        // Add bookmarks
        for(auto it = loadProj->videos.begin(); it != loadProj->videos.end(); it++){
            VideoProject* v = it->second;
            std::vector<Bookmark*> bookmarks = v->get_bookmarks();
            for(auto it2 = bookmarks.begin(); it2 != bookmarks.end(); it2++){
                Bookmark* bm = *it2;
                bookmark_view->add_bookmark(bm);
            }
        }
    }
}

/**
 * @brief MainWindow::add_project_to_tree
 * @param proj to add to tree
 * also adds all videos of the project to the tree
 */
void MainWindow::add_project_to_tree(Project* proj) {
    MyQTreeWidgetItem *project_in_tree = new MyQTreeWidgetItem(TYPE::PROJECT, QString::fromStdString(proj->name), proj->id);
    project_in_tree->setText(0, QString::fromStdString(proj->name));
    ui->project_tree->addTopLevelItem(project_in_tree);
    ui->project_tree->clearSelection();
    project_in_tree->setSelected(true);
    for(auto vid = proj->videos.begin(); vid != proj->videos.end(); ++vid){
        Video* v = vid->second->get_video();
        add_video_to_tree(v->file_path, v->id);
    }
}

/**
 * @brief MainWindow::add_video_to_tree
 * @param file_path of the video
 */
void MainWindow::add_video_to_tree(std::string file_path, ID id) {
    QTreeWidgetItem *project;
    project = ui->project_tree->selectedItems().first();
    MyQTreeWidgetItem *video_in_tree = new MyQTreeWidgetItem(TYPE::VIDEO, QString::fromStdString(file_path), id);
    video_in_tree->set_text_from_filepath(file_path);
    project->addChild(video_in_tree);
    project->setExpanded(true);
}

/**
 * @brief MainWindow::on_actionChoose_Workspace_triggered
 * Opens file explorer and requests a workspace select from user, updates
 * filehandler workspace accordingly.
 */
void MainWindow::on_actionChoose_Workspace_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Workspace"),this->fileHandler->get_work_space().absolutePath().toStdString().c_str());
    if (!dir.isEmpty()) {
        this->fileHandler->set_work_space(dir.toStdString() + "/");
        set_status_bar("New wokspace set to " + this->fileHandler->work_space);
    }
}

/**
 * @brief MainWindow::on_actionDelete_triggered
 * Deletes the selected item in the tree
 */
void MainWindow::on_actionDelete_triggered() {
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
                this->fileHandler->remove_video_from_project(my_project->id, my_item->id); // Remove video from project
            } else if (my_item->type == TYPE::PROJECT) {
                this->fileHandler->delete_project(my_item->id);
            }
            remove_item_from_tree(my_item);
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
    set_status_bar("Remove item");
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
        ui->toolBar_analysis->show();
        ui->toolBar->hide();
        ui->toolBar_overlay->hide();
    } else if (mvideo_player->is_showing_overlay()) {
        ui->toolBar_analysis->hide();
        ui->toolBar->hide();
        ui->toolBar_overlay->show();
    } else {
        ui->toolBar_analysis->hide();
        ui->toolBar->show();
        ui->toolBar_overlay->hide();
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
 * @brief MainWindow::on_video_slider_sliderPressed
 * Block slider update from video_player
 */
void MainWindow::on_video_slider_sliderPressed() {
    slider_blocked = true;
    if (!mvideo_player->is_paused()) {
        slider_paused_video = true;
        emit set_pause_video();
    }
}

/** @brief MainWindow::on_actionShow_hide_analysis_area_triggered
 * Toggles the choosing of an analysis area.
 */
void MainWindow::on_actionShow_hide_analysis_area_triggered() {
    mvideo_player->toggle_analysis_area();
    toggle_toolbar();
    if (mvideo_player->is_showing_analysis_tool()) {
        set_status_bar("Showing analysis area tool. Select your area by clicking on the video.");
    } else {
        set_status_bar("Hiding analysis area tool.");
    }
}

/**
 * @brief MainWindow::on_video_slider_sliderReleased
 * Set video playback pos to slider pos and unblock slider update
 */
void MainWindow::on_video_slider_sliderReleased() {
    int new_pos = video_slider->value();
    emit set_playback_frame(new_pos);
    video_slider->setSliderPosition(new_pos);
    slider_blocked = false;
    if (slider_paused_video) {
        paused_wait.wakeOne();
        slider_paused_video = false;
    }
}

/** @brief MainWindow::on_actionContrast_Brightness_triggered
 * Opens a window to choose contrast and brightness in.
 */
void MainWindow::on_actionContrast_Brightness_triggered() {
    float contrast = mvideo_player->get_contrast();
    int brightness = mvideo_player->get_brightness();

    // Create the texts shown in the dialog
    std::stringstream contrast_ss;
    contrast_ss << "Contrast [" << mvideo_player->CONTRAST_MIN << " – " << mvideo_player->CONTRAST_MAX <<
                   "] (default: "<< mvideo_player->CONTRAST_DEFAULT <<"): ";
    QString contrast_text = QString::fromStdString(contrast_ss.str());
    std::stringstream brightness_ss;
    brightness_ss << "Brightness [" << mvideo_player->BRIGHTNESS_MIN << " – " << mvideo_player->BRIGHTNESS_MAX <<
                     "] (default: "<< mvideo_player->BRIGHTNESS_DEFAULT <<"): ";
    QString brightness_text = QString::fromStdString(brightness_ss.str());

    // Create the dialog
    CustomDialog dialog("Contrast & Brightness", this);
    dialog.addLabel("Enter values:");
    dialog.addDblSpinBoxF(contrast_text, (float) mvideo_player->CONTRAST_MIN, (float) mvideo_player->CONTRAST_MAX,
                          &contrast, mvideo_player->CONTRAST_DECIMALS, (float) mvideo_player->CONTRAST_STEP,
                          "Choose contrast value with the input box.");
    dialog.addSpinBox(brightness_text, mvideo_player->BRIGHTNESS_MIN, mvideo_player->BRIGHTNESS_MAX,
                      &brightness, mvideo_player->BRIGHTNESS_STEP, "Choose brightness value with the input box.");

    // Show the dialog (execution will stop here until the dialog is finished)
    dialog.exec();

    if (dialog.wasCancelled()) {
        return;
    }
    mvideo_player->set_contrast(contrast);
    mvideo_player->set_brightness(brightness);
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
 * @brief MainWindow::on_actionRotate_right_triggered
 * Rotates video to the right by 90 degrees.
 */
void MainWindow::on_actionRotate_right_triggered() {
    mvideo_player->rotate_right();
    set_status_bar("Video rotated 90 degrees to the right.");
}

/**
 * @brief MainWindow::on_actionRotate_left_triggered
 * Rotates video to the left by 90 degrees.
 */
void MainWindow::on_actionRotate_left_triggered() {
    mvideo_player->rotate_left();
    set_status_bar("Video rotated 90 degrees to the left.");
}

/**
 * @brief MainWindow::on_document_list_itemClicked
 * Invoked when an item in the bookmark view has been clicked.
 * @param item The bookmark that has been clicked.
 */
void MainWindow::on_documentList_itemClicked(QListWidgetItem *item) {
    Bookmark* bookmark = (Bookmark*) item;
    if (!mvideo_player->isRunning()) {
        load_new_video(bookmark->get_video_name(), bookmark->get_frame_number(), true);
    } else if (bookmark->get_video_name() != mvideo_player->get_video_name()) {
        load_new_video(bookmark->get_video_name(), bookmark->get_frame_number(), mvideo_player->is_paused());
    } else {
        emit set_playback_frame(bookmark->get_frame_number());
    }
    set_status_bar("Jump to frame: " + to_string(bookmark->get_frame_number()) + ".");
}

/**
 * @brief MainWindow::on_action_original_size_triggered
 * Restores the video to original size and keeps it there until you activate again.
 */
void MainWindow::on_action_original_size_triggered() {
    if(mvideo_player->video_open()) {
        if (!original_size) {
            emit resize_video_frame(mvideo_player->get_video_width(), mvideo_player->get_video_height());
            original_size = true;
            ui->action_fill_screen->setEnabled(false);
            set_status_bar("Original size set. Press again to fill screen.");
        } else {
            original_size = false;
            ui->action_fill_screen->setEnabled(true);
            on_action_fill_screen_triggered();
            set_status_bar("Filling the screen with the video.");
        }

    } else {
        set_status_bar("No video loaded.");
        ui->action_original_size->toggle(); // unchecks it again
    }
}
/**
 * @brief MainWindow::on_actionInvert_analysis_area_triggered
 * Switches between choosing area for analysing and area for not analysing.
 */
void MainWindow::on_actionInvert_analysis_area_triggered() {
    mvideo_player->invert_analysis_area();
    if (mvideo_player->is_including_area()) {
        set_status_bar("Choose an area to run the analysis on.");
    } else {
        set_status_bar("Choose an area to exclude from the analysis.");
    }
}
