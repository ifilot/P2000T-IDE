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

#ifndef READTHREAD_H
#define READTHREAD_H

#include <iostream>

#include "ioworker.h"

/**
 * @brief Worker Thread responsible for reading ROM from cartridge
 */
class ReadThread : public IOWorker {

    Q_OBJECT

private:

public:
    ReadThread() {}

    ReadThread(const std::shared_ptr<SerialInterface>& _serial_interface) :
        IOWorker(_serial_interface) {}

    /**
     * @brief read the ROM from a cartridge
     *
     * This routine will be called when a thread containing this
     * class is runned
     */
    void run() override;

signals:
    /**
     * @brief signal when rom has been read
     */
    void read_result_ready();

    /**
     * @brief signal when a new sector is about to be read
     * @param sector_id
     */
    void read_block_start(unsigned int sector_id);

    /**
     * @brief signal when a new sector is read
     * @param sector_id
     */
    void read_block_done(unsigned int sector_id);
};

#endif // READTHREAD_H
