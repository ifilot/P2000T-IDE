#include "romwidget.h"

/**
 * @brief Default constructor
 * @param parent
 */
RomWidget::RomWidget(QWidget *parent) : QWidget(parent)
{
    // set widget size
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // set layout
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);

    // add table
    this->table = new QTableWidget();
    this->table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout->addWidget(this->table);
}

/**
 * @brief Populate the table with the files on the ROM
 */
void RomWidget::populate_table() {
    qDebug() << "Populating table";

    if(this->data->size() == 0) {
        return;
    }

    this->table->clear();

    // set header
    QStringList labels;
    labels << "Filename"
           << "Extension"
           << "Size"
           << "Numblocks"
           << "Launch";
    this->table->setColumnCount(labels.size());
    this->table->setHorizontalHeaderLabels(labels);

    const auto& files = this->data->get_files();
    this->table->setRowCount(files.count());
    this->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    for(int i=0; i<files.size(); i++) {
        int j = 0;
        this->table->setItem(i, j++, new QTableWidgetItem(files[i].filename));
        this->table->setItem(i, j++, new QTableWidgetItem(files[i].extension));
        this->table->setItem(i, j++, new QTableWidgetItem(tr("0x%1").arg(files[i].filesize,4,16,QChar('0'))));
        this->table->setItem(i, j++, new QTableWidgetItem(tr("%1").arg(files[i].numblocks)));

        // check if cell is valid
        if(files[i].numblocks != files[i].blocks.size()) {
            for(int j=0; j<this->table->columnCount()-1; j++) {
                this->table->item(i,j)->setForeground(QBrush(QColor(255,0,0)));
            }
            this->table->item(i,0)->setText(this->table->item(i,0)->text() + " (invalid)");
        } else {
            QPushButton* btn = new QPushButton("Launch");
            this->table->setCellWidget(i, this->table->columnCount()-1, btn);

            QSignalMapper* mapper = new QSignalMapper();
            connect(btn, SIGNAL(released()), mapper, SLOT(map()));
            mapper->setMapping(btn, i);
            connect(mapper, SIGNAL(mapped(int)), this, SLOT(slot_launch_file(int)));
        }
    }
}

/**
 * @brief Launch file from table
 */
void RomWidget::slot_launch_file(int i) {
    qDebug() << "Launching file: " << i;

    ThreadRun* runthread = new ThreadRun();
    runthread->set_mcode(this->data->build_cas(i));
    runthread->set_process_configuration(ThreadRun::ProcessConfiguration::MCODE_AS_CAS);
    connect(runthread, SIGNAL(signal_run_complete(void*)), this, SLOT(slot_run_complete(void*)));
    runthread->start();
}
