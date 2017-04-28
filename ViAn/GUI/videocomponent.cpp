#include "videocomponent.h"

VideoComponent::VideoComponent()
{
}
/**
 * @brief MainWindow::on_previous_POI_button_clicked
 * Jump back to the previous POI.
 */
void MainWindow::on_previous_POI_button_clicked() {
    if (mvideo_player->is_paused()) {
        set_status_bar("Went back to the previous POI");
        //will be added when functionality is in place
        //emit previous_video_POI();
    } else {
        set_status_bar("Needs to be paused");
    }
}

/**
 * @brief MainWindow::on_next_POI_button_clicked
 * Jump forward to the next POI.
 */
void MainWindow::on_next_POI_button_clicked() {
    if (mvideo_player->is_paused()) {
        set_status_bar("Went forward to the next POI");
        //will be added when functionality is in place
        //emit next_video_POI();
    } else {
        set_status_bar("Needs to be paused");
    }
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

/**
 * @brief MainWindow::on_action_rotate_right_triggered
 * Rotates video to the right by 90 degrees.
 */
void MainWindow::on_action_rotate_right_triggered() {
    mvideo_player->rotate_right();
    set_status_bar("Video rotated 90 degrees to the right.");
}

/**
 * @brief MainWindow::on_action_rotate_left_triggered
 * Rotates video to the left by 90 degrees.
 */
void MainWindow::on_action_rotate_left_triggered() {
    mvideo_player->rotate_left();
    set_status_bar("Video rotated 90 degrees to the left.");
}

/**
 * @brief MainWindow::play_video
 *  Loads selected video, flips playbutton to pause
 *  plays video from beginning
 */
void MainWindow::play_video() {
    MyQTreeWidgetItem *my_project;
    my_project = (MyQTreeWidgetItem*)ui->project_tree->selectedItems().first();

    if (mvideo_player->is_paused())
        paused_wait.wakeOne();

    if (mvideo_player->isRunning()) {
        emit set_stop_video(); //This signal will make the QThread finish executing
        mvideo_player = new video_player(&mutex, &paused_wait, ui->video_frame);
        setup_video_player(mvideo_player);
    }
    mvideo_player->load_video(my_project->name.toStdString());

    //Used for rescaling the source image for video playback
    emit resize_video_frame(ui->video_frame->width(),ui->video_frame->height());
    enable_video_buttons();
    icon_on_button_handler->set_icon("pause", ui->play_pause_button);
    video_slider->setMaximum(mvideo_player->get_num_frames() - 1);
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
    //Will be added when functionality is in place
    /*QObject::connect(this, SIGNAL(next_video_POI()),
                     mplayer, SLOT(next_POI()));
    QObject::connect(this, SIGNAL(prev_video_POI()),
                     mplayer, SLOT(previous_POI()));*/
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
