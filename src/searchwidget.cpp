#include "searchwidget.h"

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout();
    this->setLayout(layout);

    this->line_edit = new QLineEdit();
    this->button_search = new QPushButton("Search");
    this->button_cancel = new QPushButton("Cancel");

    layout->addWidget(this->line_edit);
    layout->addWidget(this->button_search);
    layout->addWidget(this->button_cancel);

    connect(this->button_search, SIGNAL(released()), this, SIGNAL(search()));
    connect(this->button_cancel, SIGNAL(released()), this, SLOT(hide()));
    connect(this->line_edit, SIGNAL(returnPressed()), this, SIGNAL(search()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(hide()));

    this->setVisible(false);
}

void SearchWidget::show_search_widget() {
    this->setVisible(true);
    this->line_edit->setFocus();
}

void SearchWidget::hide() {
    this->setVisible(false);
    this->line_edit->clear();
    emit(search_done());
}
