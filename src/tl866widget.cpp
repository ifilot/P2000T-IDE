#include "tl866widget.h"

TL866Widget::TL866Widget(QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->build_interface();

    // establish connections
    connect(this->button_read_chip, SIGNAL(released()), this, SLOT(slot_read_chip()));
    connect(this->button_write_chip, SIGNAL(released()), this, SLOT(slot_write_chip()));
}

void TL866Widget::build_interface() {
    QVBoxLayout* top_layout = new QVBoxLayout();
    this->setLayout(top_layout);

    QWidget* button_container = new QWidget();
    top_layout->addWidget(button_container);
    QHBoxLayout* button_layout = new QHBoxLayout();
    button_container->setLayout(button_layout);

    this->button_read_chip = new QPushButton("Read chip");
    button_layout->addWidget(this->button_read_chip);

    this->button_write_chip = new QPushButton("Write chip");
    button_layout->addWidget(this->button_write_chip);

    this->progress_bar = new QProgressBar();
    top_layout->addWidget(this->progress_bar);
}

void TL866Widget::slot_read_chip() {
    qDebug() << "Launching chip read process.";
    this->button_read_chip->setEnabled(false);
    this->button_write_chip->setEnabled(false);
    this->programmer_thread = std::make_unique<ThreadTL866>();
    this->programmer_thread->set_operation(0); // read operation
    this->progress_bar->setMinimum(0);
    this->progress_bar->setMaximum(100);
    connect(this->programmer_thread.get(), SIGNAL(signal_read_done(void*)), this, SLOT(slot_read_done(void*)));
    connect(this->programmer_thread.get(), SIGNAL(signal_progress(int)), this->progress_bar, SLOT(setValue(int)));
    this->programmer_thread->start();
}

void TL866Widget::slot_write_chip() {
    qDebug() << "Launching chip write process.";
    emit(signal_get_data());
    this->button_read_chip->setEnabled(false);
    this->button_write_chip->setEnabled(false);
    this->progress_bar->setMinimum(0);
    this->progress_bar->setMaximum(100);
    this->programmer_thread = std::make_unique<ThreadTL866>();
    this->programmer_thread->set_operation(1); // write operation
    this->programmer_thread->set_data(this->flash_data);
    connect(this->programmer_thread.get(), SIGNAL(signal_write_done(void*)), this, SLOT(slot_write_done(void*)));
    connect(this->programmer_thread.get(), SIGNAL(signal_progress(int)), this->progress_bar, SLOT(setValue(int)));
    this->programmer_thread->start();
}

void TL866Widget::slot_read_done(void*) {
    this->button_read_chip->setEnabled(true);
    this->button_write_chip->setEnabled(true);
    this->progress_bar->setValue(this->progress_bar->maximum());
    this->read_data = this->programmer_thread->get_data();
    this->log_data = this->programmer_thread->get_output();
    emit(signal_data_read());
    emit(signal_log_read());
    this->programmer_thread.reset(); // clean object
}

void TL866Widget::slot_write_done(void*) {
    qDebug() << "Done writing to chip.";
    this->button_read_chip->setEnabled(true);
    this->button_write_chip->setEnabled(true);
    this->progress_bar->setValue(this->progress_bar->maximum());
    this->log_data = this->programmer_thread->get_output();
    emit(signal_log_read());
    this->programmer_thread.reset(); // clean object
}
