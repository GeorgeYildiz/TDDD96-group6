#ifndef BOOKMARKVIEW_H
#define BOOKMARKVIEW_H

#include <QListWidget>

class BookmarkView {
public:
    BookmarkView(QListWidget* parent);
    void add_bookmark(std::string file_path);
    int get_num_bookmarks();
private:
    QString get_input_text();

    const int BOOKMARK_HEIGHT = 64;
    const int TEXT_EDIT_HEIGHT = 54;

    QListWidget* view;
};

#endif // BOOKMARKVIEW_H