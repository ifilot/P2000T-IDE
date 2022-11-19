#include "fileallocationtablep2000t.h"

FileAllocationTableP2000t::FileAllocationTableP2000t(const QByteArray& _data) {
    // set data
    this->data = _data;

    // start analysis
    this->valid_metaheaders();
}

bool FileAllocationTableP2000t::valid_metaheaders() {
    for(unsigned int b=0; b<8; b++) {
        for(unsigned int i=0; i<60; i++) {
            if((unsigned int)this->data[b * 0x10000 + i*0x40 + 0x100] != b) {
                qCritical() << "Invalid byte encountered for Bank #" << b << ", Block #" << i << ".";
                return false;
            }

            if((unsigned int)this->data[b * 0x10000 + i*0x40 + 0x102] != (i * 4 + 0x10) ||
               (unsigned int)this->data[b * 0x10000 + i*0x40 + 0x101] != 0) {
                qCritical() << "Invalid address encountered for Bank #" << b << ", Block #" << i << ".";
                return false;
            }
        }
    }

    return true;
}
