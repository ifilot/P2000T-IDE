/****************************************************************************
 *                                                                          *
 *   GBCR                                                                   *
 *   Copyright (C) 2021 Ivo Filot <ivo@ivofilot.nl>                         *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as         *
 *   published by the Free Software Foundation, either version 3 of the     *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public license      *
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 *                                                                          *
 ****************************************************************************/

#include "readthread.h"

/**
 * @brief read the ROM from a cartridge
 *
 * This routine will be called when a thread containing this
 * class is runned
 */
void ReadThread::run() {
    unsigned int sector_counter = 0;

    this->serial_interface->open_port();

    // read the first 16 kb
    for(unsigned int i=0; i<64; i++) {  // 64 blocks of 256 bytes each
        emit(read_block_start(sector_counter));
        auto sectordata = this->serial_interface->read_block(i);
        this->data.append(sectordata);
        emit(read_block_done(sector_counter));
        sector_counter++;
    }

    this->serial_interface->close_port();
    emit(read_result_ready());
}
