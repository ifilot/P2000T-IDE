#include "fileallocationtablep2000t.h"

/**
 * @brief Default constructor
 * @param raw ROM data
 */
FileAllocationTableP2000t::FileAllocationTableP2000t(const QByteArray& _data) {
    // set data
    this->data = _data;

    // start analysis
    this->index_files();
}

/**
 * @brief Indexes all the files
 */
void FileAllocationTableP2000t::index_files() {
    if(this->data.size() == 0) {
        return;
    }

    // collect all file vectors
    for(uint8_t i=0; i<8; i++) { // loop over banks
        for(uint8_t j=0; j<60; j++) { // loop over banks
            if((uint8_t)data[i*0x10000 + j] != 0xFF) {
                this->file_vectors.append(i*256+(uint8_t)data[i*0x10000 + j]);
            }
        }
    }

    // loop over file vectors and capture string names
    for(int i=0; i<this->file_vectors.size(); i++) {
        uint8_t bankid = this->file_vectors[i] >> 8;
        uint8_t blockid = this->file_vectors[i] & 0xFF;
        static const uint32_t bankoffset = 0x100;
        const uint32_t start = bankid * 0x10000 + blockid * 0x40 + bankoffset;

        // build file
        static const uint32_t descoffset01 = 0x26;
        static const uint32_t descoffset02 = 0x37;
        RomFile file;

        // grab filename
        const uint32_t dpos01 = start + descoffset01;
        const uint32_t dpos02 = start + descoffset02;
        QString desc = this->data.mid(dpos01, 8) + this->data.mid(dpos02, 8);
        file.filename = desc;

        // grab extension
        file.extension = this->data.mid(start + 0x2E,3);

        // grab filesize
        const uint32_t length = start + 0x22;
        file.filesize = data[length] + data[length+1] * 256;

        // loop over blocks
        file.blocks.append(bankid * 256 + blockid);
        uint8_t nextbank = this->data[start + 3];
        uint8_t nextblock = this->data[start + 4];
        while(nextbank != 0xFF && nextblock != 0xFF) {
            file.blocks.append(nextbank * 256 + nextblock);
            uint32_t newstart = nextbank * 0x10000 + nextblock * 0x40 + 0x100;
            nextbank = this->data[newstart + 3];
            nextblock = this->data[newstart + 4];
        }
        file.numblocks = this->data[start + 0xA];

        this->files.append(file);
    }
}

/**
 * @brief Build a CAS file given file index
 * @param file index
 * @return raw CAS file
 */
QByteArray FileAllocationTableP2000t::build_cas(int i) {
    if(i < 0 || i >= this->files.size()) {
        throw std::runtime_error("Invalid file index given!");
    }

    RomFile file = this->files[i];
    auto blocks = file.blocks;

    QByteArray casfile;

    for(uint16_t block : blocks) {
        uint8_t bankid = block >> 8;
        uint8_t blockid = block & 0xFF;

        casfile.append(0x30, (uint8_t)0x00);
        casfile.append(this->data.mid(bankid * 0x10000 + 0x100 + blockid * 0x40 + 0x20, 0x20));
        casfile.append(0x100 - 0x50, (uint8_t)0x00);
        casfile.append(this->data.mid(bankid * 0x10000 + 0x1000 + blockid * 0x400, 0x400));
    }

    return casfile;
}
