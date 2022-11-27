#ifndef ROMWIDGET_H
#define ROMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QSignalMapper>

#include "threadrun.h"

#include "fileallocationtablep2000t.h"

/**
 * @brief Widget that lists all the files on a P2000T FAT type ROM
 */
class RomWidget : public QWidget
{
    Q_OBJECT

private:
    std::unique_ptr<FileAllocationTableP2000t> data;    // pointer to FAT object
    QTableWidget* table;                                // table showing all files

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit RomWidget(QWidget *parent = nullptr);

    /**
     * @brief set raw data, index files and build table
     * @param data
     */
    inline void set_data(const QByteArray& data) {
        this->data = std::make_unique<FileAllocationTableP2000t>(data);
        this->populate_table();
    }

private:
    /**
     * @brief Populate the table with the files on the ROM
     */
    void populate_table();

private slots:
    /**
     * @brief Launch file from table
     */
    void slot_launch_file(int);

};

#endif // ROMWIDGET_H
