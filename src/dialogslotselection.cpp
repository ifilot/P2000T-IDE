#include "dialogslotselection.h"

DialogSlotSelection::DialogSlotSelection()
{
    // build layout
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(new QLabel("Please select a ROM slot"));

    // add container for ROM slots
    QGroupBox* groupbox = new QGroupBox("ROM slots");
    layout->addWidget(groupbox);
    QGridLayout* grid = new QGridLayout();
    groupbox->setLayout(grid);

    // add buttons
    for(unsigned int i=0; i<4; i++) {
        for(unsigned int j=0; j<8; j++) {
            QPushButton* btn = new QPushButton(tr("%1").arg(i*8+j+1));
            grid->addWidget(btn, i, j);
            connect(btn, SIGNAL(released()), this, SLOT(slot_select_romslot()));
        }
    }
}

void DialogSlotSelection::slot_select_romslot() {
    QPushButton* btn = qobject_cast<QPushButton *>(sender());
    this->slot_id = btn->text().toInt() - 1;
    this->accept();
}
