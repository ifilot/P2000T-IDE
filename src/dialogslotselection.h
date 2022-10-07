#ifndef DIALOGSLOTSELECTION_H
#define DIALOGSLOTSELECTION_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>

class DialogSlotSelection : public QDialog
{
    Q_OBJECT
private:
    int slot_id = -1;

public:
    DialogSlotSelection();

    int get_slot_id() const {
        return this->slot_id;
    }

private slots:
    void slot_select_romslot();
};

#endif // DIALOGSLOTSELECTION_H
