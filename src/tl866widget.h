#ifndef TL866WIDGET_H
#define TL866WIDGET_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>

#include "threadtl866.h"

class TL866Widget : public QWidget
{
    Q_OBJECT
private:
    QPushButton* button_read_chip;
    QPushButton* button_write_chip;
    QProgressBar* progress_bar;

    std::unique_ptr<ThreadTL866> programmer_thread;
    QByteArray flash_data;
    QByteArray read_data;
    QByteArray log_data;

public:
    explicit TL866Widget(QWidget *parent = nullptr);

    inline void set_flash_data(const QByteArray& _data) {
        this->flash_data = _data;
    }

    inline const QByteArray& get_data() const {
        return this->read_data;
    }

    inline const auto& get_log_data() const {
        return this->log_data;
    }

private:
    void build_interface();

signals:
    /**
     * @brief Request to read data from this object
     */
    void signal_data_read();

    /**
     * @brief Request to provide this object data
     */
    void signal_get_data();

    /**
     * @brief Request to read log data from this object
     */
    void signal_log_read();

private slots:
    void slot_read_chip();

    void slot_write_chip();

    void slot_read_done(void*);

    void slot_write_done(void*);
};

#endif // TL866WIDGET_H
