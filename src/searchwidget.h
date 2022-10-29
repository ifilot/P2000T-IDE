#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QShortcut>

class SearchWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit* line_edit;
    QPushButton* button_search;
    QPushButton* button_cancel;

public:
    explicit SearchWidget(QWidget *parent = nullptr);

    inline auto* get_line_edit_ptr() {
        return this->line_edit;
    }

signals:
    void search();

    void search_done();

public slots:
    void show_search_widget();

    void hide();
};

#endif // SEARCHWIDGET_H
