#include "serialwidget.h"

SerialWidget::SerialWidget(QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->layout_serial_vertical = new QVBoxLayout();
    this->setLayout(layout_serial_vertical);

    // build interfaces
    this->build_serial_interface(this->layout_serial_vertical);
    this->build_data_interface(this->layout_serial_vertical);

    // make connections
    connect(this->button_scan_ports, SIGNAL (released()), this, SLOT (scan_com_devices()));
    connect(this->button_select_serial, SIGNAL (released()), this, SLOT (select_com_port()));
    connect(this->button_read_cartridge, SIGNAL (released()), this, SLOT (read_cartridge()));
    connect(this->button_write_cartridge, SIGNAL(released()), this, SLOT(flash_rom()));
}

/**
 * @brief Scan all communication ports to populate drop-down box
 */
void SerialWidget::scan_com_devices() {
    // clear all previous data
    this->combobox_serial_ports->clear();
    this->port_identifiers.clear();

    // pattern to recognise GBCR (same ids as Arduino Leonardo)
    static const std::vector<std::pair<uint16_t, uint16_t> > patterns = {
        std::make_pair<uint16_t, uint16_t>(0x2341, 0x36),   // Arduino Leonardo / 32u4
        std::make_pair<uint16_t, uint16_t>(0x0403, 0x6001)  // FTDI FT232RL
    };

    // get communication devices
    QSerialPortInfo serial_port_info;
    QList<QSerialPortInfo> port_list = serial_port_info.availablePorts();
    std::unordered_map<std::string, std::pair<uint16_t, uint16_t> > ports;
    QStringList device_descriptors;
    qInfo() << "Discovered COM ports.";
    for(int i=0; i<port_list.size(); i++) {
        auto ids = std::make_pair<uint16_t,uint16_t>(port_list[i].vendorIdentifier(), port_list[i].productIdentifier());
        for(int j=0; j<patterns.size(); j++) {
            if(ids == patterns[j]) {
                ports.emplace(port_list[i].portName().toStdString(), ids);
                device_descriptors.append(port_list[i].description());
                qInfo() << port_list[i].portName().toStdString().c_str()
                        << QString("pid=0x%1, vid=0x%2,").arg(port_list[i].vendorIdentifier(),2,16).arg(port_list[i].productIdentifier(),2,16).toStdString().c_str()
                        << QString("descriptor=\"%1\",").arg(port_list[i].description()).toStdString().c_str()
                        << QString("serial=\"%1\"").arg(port_list[i].serialNumber()).toStdString().c_str();
            }
        }
    }

    // populate drop-down menu with valid ports
    for(const auto& item : ports) {
        this->combobox_serial_ports->addItem(item.first.c_str());
        this->port_identifiers.push_back(item.second);
    }

    // if more than one option is available, enable the button
    if(this->combobox_serial_ports->count() > 0) {
        this->button_select_serial->setEnabled(true);
    }


    if(port_identifiers.size() == 1) {
        this->signal_emit_statusbar_message(tr("Auto-selecting ") + this->combobox_serial_ports->itemText(0) + tr(" (vid and pid match board)."));
        this->combobox_serial_ports->setCurrentIndex(0);
        this->select_com_port();
    } else if(port_identifiers.size() > 1) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.setText(tr(
              "There are at least %1 devices that share the same id. Please ensure that only a single P2k-device is plugged in."
              " If multiple devices are plugged in, ensure you select the correct port. Please also note that the device id overlaps"
              " with the one from the Arduino Leonardo bootloader. If you have an Arduino Leonardo or compatible device plugged in,"
              " take care to unplug it or carefully select the correct port."
        ).arg(port_identifiers.size()));
        //msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
        msg_box.exec();
    } else {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.setText("Could not find a communication port with a matching id. Please make sure the P2k-device is plugged in.");
        //msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
        msg_box.exec();
    }
}

/**
 * @brief Select communication port for serial to 32u4
 */
void SerialWidget::select_com_port() {
    auto port_id = this->port_identifiers[this->combobox_serial_ports->currentIndex()];

    if(port_id == std::make_pair<uint16_t, uint16_t>(0x2341, 0x36)) {          // Arduino Leonardo / 32u4
        qDebug() << "Connecting to 32u4; setting baud rate to 115200.";
        this->serial_interface = std::make_shared<SerialInterface>(this->combobox_serial_ports->currentText().toStdString(), 115200);
    } else {
        throw std::runtime_error("Invalid port id.");
    }

    this->serial_interface->open_port();
    std::string board_info = this->serial_interface->get_board_info();
    this->serial_interface->close_port();
    this->label_serial->setText(tr("Port: ") + this->combobox_serial_ports->currentText());
    this->label_board_id->setText(tr("Board id: ") + tr(board_info.c_str()));
    if(board_info.substr(0,8) == "P2k-32u4") {
        this->button_read_cartridge->setEnabled(true);
        this->button_write_cartridge->setEnabled(true);
    }
}

/*****************************************************************************************************
 *
 * CARTRIDGE READ FUNCTIONS
 *
 *****************************************************************************************************/

/**
 * @brief Read data from chip
 */
void SerialWidget::read_cartridge() {
    DialogSlotSelection dialog;
    int res = dialog.exec();
    if(res != QDialog::Accepted) {
        qDebug() << "Cancelled operation.";
        return;
    }

    int slot_id = dialog.get_slot_id();
    qDebug() << "Selecting slot " << slot_id;

    this->timer1.start();
    this->num_blocks = 64;

    // disable all buttons so that the user cannot interrupt this task
    this->disable_all_buttons();

    // dispatch thread
    this->readerthread = std::make_unique<ReadThread>(this->serial_interface);
    this->readerthread->set_rom_slot(slot_id);
    this->readerthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    connect(this->readerthread.get(), SIGNAL(read_result_ready()), this, SLOT(read_result_ready()));
    connect(this->readerthread.get(), SIGNAL(read_block_start(uint)), this, SLOT(read_block_start(uint)));
    connect(this->readerthread.get(), SIGNAL(read_block_done(uint)), this, SLOT(read_block_done(uint)));
    this->readerthread->start();
}

/**
 * @brief Slot to accept when a sector is ready
 */
void SerialWidget::read_block_start(unsigned int sector_id) {
    this->progress_bar_load->setMaximum(64);
    this->progress_bar_load->setValue(sector_id);
}

/**
 * @brief Slot to accept when a sector is ready
 */
void SerialWidget::read_block_done(unsigned int sector_id) {
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_sector = seconds_passed / (double)(sector_id+1);
    double seconds_remaining = seconds_per_sector * (this->num_blocks - sector_id - 1);
    if(sector_id < (this->num_blocks - 1)) {
        this->signal_emit_statusbar_message(QString("%1 sector %2 / %3 : %4 seconds remaining.").arg("Reading").arg(sector_id+1).arg(this->num_blocks).arg(seconds_remaining));
    }
    this->progress_bar_load->setValue(sector_id+1);
}

/*
 * @brief Signal that a read operation is finished
 */
void SerialWidget::read_result_ready() {
    this->progress_bar_load->setValue(this->progress_bar_load->maximum());
    this->data = this->readerthread->get_data();
    this->readerthread.reset(); // delete object
    this->signal_data_read();
    this->enable_all_buttons();
}

/*****************************************************************************************************
 *
 * CARTRIDGE FLASH FUNCTIONS
 *
 *****************************************************************************************************/

/**
 * @brief Put rom on flash cartridge
 */
void SerialWidget::flash_rom() {
    DialogSlotSelection dialog;
    int res = dialog.exec();
    if(res != QDialog::Accepted) {
        qDebug() << "Cancelled operation.";
        return;
    }
    int slot_id = dialog.get_slot_id();

    // dispatch thread
    this->timer1.start();

    // perform data request
    this->signal_get_data();

    // pad data with zeros
    QByteArray padding;
    padding.fill(0, 0x4000 - this->flash_data.size());
    this->flash_data += padding;

    this->progress_bar_load->setMaximum(64);
    this->flashthread = std::make_unique<FlashThread>(this->serial_interface);
    this->flashthread->set_rom_slot(slot_id);
    this->flashthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    this->flashthread->set_data(this->flash_data);

    connect(this->flashthread.get(), SIGNAL(flash_result_ready()), this, SLOT(flash_result_ready()));
    connect(this->flashthread.get(), SIGNAL(flash_block_start(uint)), this, SLOT(flash_block_start(uint)));
    connect(this->flashthread.get(), SIGNAL(flash_block_done(uint)), this, SLOT(flash_block_done(uint)));
    connect(this->flashthread.get(), SIGNAL(flash_chip_id_error(uint)), this, SLOT(flash_chip_id_error(uint)));
    flashthread->start();

    // disable all buttons
    this->disable_all_buttons();
}

/**
 * @brief Slot to indicate that a page is about to be written
 */
void SerialWidget::flash_block_start(unsigned int page_id) {
    this->progress_bar_load->setValue(page_id);
}

/**
 * @brief Slot to accept that a page is written
 */
void SerialWidget::flash_block_done(unsigned int page_id) {
    unsigned int num_pages = 64;
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_page = seconds_passed / (double)(page_id+1);
    double seconds_remaining = seconds_per_page * (num_pages - page_id - 1);
    if(page_id < (num_pages - 1)) {
        this->signal_emit_statusbar_message(QString("%1 page %2 / %3 : %4 seconds remaining.").arg("Flashing ").arg(page_id+1).arg(num_pages).arg(seconds_remaining));
    }
    this->progress_bar_load->setValue(page_id+1);
}

/*
 * @brief Signal that a flash operation is finished
 */
void SerialWidget::flash_result_ready() {
    this->progress_bar_load->setValue(this->progress_bar_load->maximum());
    this->signal_emit_statusbar_message("Ready - Done flashing in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds.");

    // dispatch thread
    this->timer1.restart();
    this->readerthread = std::make_unique<ReadThread>(this->serial_interface);
    this->readerthread->set_rom_slot(this->flashthread->get_rom_slot());
    this->readerthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());

    // set progress bar
    this->progress_bar_load->reset();
    this->num_blocks = 64;
    this->progress_bar_load->setMaximum(64);

    connect(this->readerthread.get(), SIGNAL(read_result_ready()), this, SLOT(verify_result_ready()));
    connect(this->readerthread.get(), SIGNAL(read_block_start(uint)), this, SLOT(verify_block_start(uint)));
    connect(this->readerthread.get(), SIGNAL(read_block_done(uint)), this, SLOT(verify_block_done(uint)));
    readerthread->start();
}

/*
 * @brief Response that the chip id could not be verified
 */
void SerialWidget::flash_chip_id_error(unsigned int chip_id) {
    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Warning);
    msg_box.setText(tr("The chip id (%1) does not match the proper value for a SST39SF0x0 chip. Please verify that you inserted"
                       " and/or selected the right FLASH cartridge. If so, resocket the cartridge and try again.").arg(chip_id,0,16));
    msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
    msg_box.exec();

    // reset flash button
    this->progress_bar_load->setEnabled(true);
}

/**
 * @brief Slot to accept when a sector is about to be verified
 */
void SerialWidget::verify_block_start(unsigned int sector_id) {
    this->progress_bar_load->setValue(sector_id);
}

/**
 * @brief Slot to accept when a sector is verified
 */
void SerialWidget::verify_block_done(unsigned int sector_id) {
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_sector = seconds_passed / (double)(sector_id+1);
    double seconds_remaining = seconds_per_sector * (this->num_blocks - sector_id - 1);
    if(sector_id < (this->num_blocks - 1)) {
        this->signal_emit_statusbar_message(QString("%1 sector %2 / %3 : %4 seconds remaining.").arg("Verifying ").arg(sector_id+1).arg(this->num_blocks).arg(seconds_remaining));
    }
    this->progress_bar_load->setValue(sector_id+1);
}

/*
 * @brief Signal that a verified operation is finished
 */
void SerialWidget::verify_result_ready() {
    this->progress_bar_load->setValue(this->num_blocks);
    QByteArray verify_data = this->readerthread->get_data();
    this->readerthread.reset(); // delete object

    if(verify_data == this->flash_data) {
        this->signal_emit_statusbar_message("Ready - Done verification in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds.");
        QMessageBox msg_box(QMessageBox::Information,
                "Flash complete",
                "Cartridge was successfully flashed. Data integrity verified.",
                QMessageBox::Ok, this);
        msg_box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        msg_box.exec();
    } else {
        QMessageBox msg_box(QMessageBox::Critical,
                "Error",
                "Data integrity could not be verified. Please try to reflash the cartridge. It might help to resocket the flash cartridge.",
                QMessageBox::Ok, this);
        msg_box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        msg_box.exec();
    }

    // re-enable all buttons when data is read
    this->enable_all_buttons();
}

/*****************************************************************************************************
 *
 * GUI FUNCTIONS
 *
 *****************************************************************************************************/

void SerialWidget::build_serial_interface(QVBoxLayout* target_layout) {
    QGroupBox* serial_container = new QGroupBox("Serial interface");
    target_layout->addWidget(serial_container);
    QHBoxLayout *serial_layout = new QHBoxLayout();
    serial_container->setLayout(serial_layout);
    QLabel* comportlabel = new QLabel(tr("COM port"));
    serial_layout->addWidget(comportlabel);
    this->combobox_serial_ports = new QComboBox(this);
    serial_layout->addWidget(this->combobox_serial_ports);
    this->button_scan_ports = new QPushButton(tr("Scan"));
    serial_layout->addWidget(this->button_scan_ports);
    this->button_select_serial = new QPushButton(tr("Select"));
    this->button_select_serial->setEnabled(false);
    serial_layout->addWidget(this->button_select_serial);

    // create interface for currently selected com port
    QWidget* serial_selected_container = new QWidget();
    QHBoxLayout *serial_selected_layout = new QHBoxLayout();
    serial_selected_container->setLayout(serial_selected_layout);
    this->label_serial = new QLabel(tr("Please select a COM port from the menu above"));
    serial_selected_layout->addWidget(this->label_serial);
    this->label_board_id = new QLabel();
    serial_selected_layout->addWidget(this->label_board_id);
    layout_serial_vertical->addWidget(serial_selected_container);
}

void SerialWidget::build_data_interface(QVBoxLayout* target_layout) {
    // read and store data
    QGroupBox* data_container = new QGroupBox("Cartridge Interface");
    QGridLayout *data_layout = new QGridLayout();
    data_container->setLayout(data_layout);
    target_layout->addWidget(data_container);
    this->button_read_cartridge = new QPushButton(tr("Read from cartridge"));
    data_layout->addWidget(this->button_read_cartridge, 0, 0);
    this->button_read_cartridge->setEnabled(false);
    this->button_write_cartridge = new QPushButton(tr("Write to cartridge"));
    data_layout->addWidget(this->button_write_cartridge, 0, 1);
    this->button_write_cartridge->setEnabled(false);

    // build progress indicator
    this->progress_bar_load = new QProgressBar();
    data_layout->addWidget(this->progress_bar_load, 2, 0, 1, 2);
}

/**
 * @brief Disable all interface boxes
 */
void SerialWidget::disable_all_buttons() {
    this->button_select_serial->setEnabled(false);
    this->button_scan_ports->setEnabled(false);
    this->button_read_cartridge->setEnabled(false);
    this->button_write_cartridge->setEnabled(false);
}

/**
 * @brief Enable all interface boxes
 */
void SerialWidget::enable_all_buttons() {
    this->button_select_serial->setEnabled(true);
    this->button_scan_ports->setEnabled(true);
    this->button_read_cartridge->setEnabled(true);
    this->button_write_cartridge->setEnabled(true);
}
