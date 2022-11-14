#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QGroupBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QElapsedTimer>

#include "serial_interface.h"
#include "readthread.h"
#include "flashthread.h"
#include "dialogslotselection.h"

class SerialWidget : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout* layout_serial_vertical;
    QPushButton* button_scan_ports;
    QPushButton* button_select_serial;

    QLabel* label_serial;
    QLabel* label_board_id;

    QPushButton* button_read_cartridge;
    QPushButton* button_write_cartridge;
    QProgressBar* progress_bar_load;

    QComboBox* combobox_serial_ports;
    std::vector<std::pair<uint16_t, uint16_t>> port_identifiers;
    std::unique_ptr<ReadThread> readerthread;
    std::unique_ptr<FlashThread> flashthread;
    std::shared_ptr<SerialInterface> serial_interface;

    QByteArray data;            // rom data
    QByteArray flash_data;      // data to be flashed
    unsigned int num_blocks;    // number of blocks to read

    // time operations
    QElapsedTimer timer1;
    QElapsedTimer timer2;

public:
    explicit SerialWidget(QWidget *parent = nullptr);

    /**
     * @brief Get the data from this object
     * @return data
     */
    const auto& get_data() const {
        return this->data;
    }

    /**
     * @brief Set flash data
     * @param _data
     */
    void set_flash_data(const QByteArray& _data) {
        this->flash_data = _data;
    }

private:
    void build_serial_interface(QVBoxLayout* target_layout);

    void build_data_interface(QVBoxLayout* target_layout);

    void disable_all_buttons();

    void enable_all_buttons();

private slots:
    /****************************************************************************
     *  SIGNALS :: COMMUNICATION INTERFACE ROUTINES
     ****************************************************************************/

    /**
     * @brief Scan all communication ports to populate drop-down box
     */
    void scan_com_devices();

    /**
     * @brief Select communication port for serial to 32u4
     */
    void select_com_port();

    /****************************************************************************
     *  SIGNALS :: READ ROM ROUTINES
     ****************************************************************************/

    /**
     * @brief Read data from chip
     */
    void read_cartridge();

    /**
     * @brief Slot to indicate that a sector is about to be read / written
     */
    void read_block_start(unsigned int sector_id);

    /**
     * @brief Slot to accept that a sector is read / written
     */
    void read_block_done(unsigned int sector_id);

    /*
     * @brief Signal that a read operation is finished
     */
    void read_result_ready();

    /****************************************************************************
     *  SIGNALS :: FLASH ROM
     ****************************************************************************/

    /**
     * @brief Put rom on flash cartridge
     */
    void flash_rom();

    /**
     * @brief Slot to indicate that a page is about to be written
     */
    void flash_block_start(unsigned int page_id);

    /**
     * @brief Slot to accept that a page is written
     */
    void flash_block_done(unsigned int page_id);

    /*
     * @brief Signal that a flash operation is finished
     */
    void flash_result_ready();

    /*
     * @brief Signal that a flash operation is finished
     */
    void flash_chip_id_error(unsigned int chip_id);

    /**
     * @brief Slot to accept when a sector is about to be verified
     */
    void verify_block_start(unsigned int page_id);

    /**
     * @brief Slot to accept when a sector is verified
     */
    void verify_block_done(unsigned int page_id);

    /*
     * @brief Signal that a verified operation is finished
     */
    void verify_result_ready();

signals:
    void signal_emit_statusbar_message(const QString& str);

    /**
     * @brief Request that data is extracted from this widget and put into the hex viewer
     */
    void signal_data_read();

    /**
     * @brief Request for data to be sent
     */
    void signal_get_data();
};

#endif // SERIALWIDGET_H
