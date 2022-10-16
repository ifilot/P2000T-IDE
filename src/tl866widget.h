#ifndef TL866WIDGET_H
#define TL866WIDGET_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "threadtl866.h"

class TL866Widget : public QWidget
{
    Q_OBJECT
private:
    QPushButton* button_read_chip;
    QPushButton* button_write_chip;

    std::unique_ptr<ThreadTL866> programmer_thread;

public:
    explicit TL866Widget(QWidget *parent = nullptr);

private:
    void build_interface();

signals:

private slots:
    void slot_read_chip();

    void slot_write_chip();

    void slot_read_done(void*);

};

#endif // TL866WIDGET_H
