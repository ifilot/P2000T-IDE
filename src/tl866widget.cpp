#include "tl866widget.h"

TL866Widget::TL866Widget(QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->build_interface();

    // establish connections
    connect(this->button_read_chip, SIGNAL(released()), this, SLOT(slot_read_chip()));
    connect(this->button_write_chip, SIGNAL(released()), this, SLOT(slot_write_chip()));
}

void TL866Widget::build_interface() {
    QHBoxLayout* layout = new QHBoxLayout();
    this->setLayout(layout);

    this->button_read_chip = new QPushButton("Read chip");
    layout->addWidget(this->button_read_chip);

    this->button_write_chip = new QPushButton("Write chip");
    layout->addWidget(this->button_write_chip);
}

void TL866Widget::slot_read_chip() {
    // dispatch thread
    this->button_read_chip->setEnabled(false);
    this->button_write_chip->setEnabled(false);
    this->programmer_thread = std::make_unique<ThreadTL866>();
    this->programmer_thread->set_operation(0); // read operation
    connect(this->programmer_thread.get(), SIGNAL(signal_read_done(void*)), this, SLOT(slot_read_done(void*)));
    this->programmer_thread->start();
}

void TL866Widget::slot_write_chip() {

}

void TL866Widget::slot_read_done(void*) {
    this->button_read_chip->setEnabled(true);
    this->button_write_chip->setEnabled(true);
}
