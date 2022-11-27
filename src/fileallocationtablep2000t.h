#ifndef FILEALLOCATIONTABLEP2000T_H
#define FILEALLOCATIONTABLEP2000T_H

#include <QByteArray>
#include <QDebug>
#include <stdexcept>

/**
 * @brief Contains metadata for each file
 */
class RomFile {

public:
    QString filename;
    uint8_t numblocks = 0;
    uint16_t filesize = 0;
    QString extension;
    QVector<uint16_t> blocks;
};

/**
 * @brief Provides file handling for raw data
 */
class FileAllocationTableP2000t {

private:
    QByteArray data;                    // raw data
    QVector<uint16_t> file_vectors;     // pointers to where files start
                                        // upper byte = bank, lower byte = block
    QVector<RomFile> files;             // file metadata

public:
    /**
     * @brief Default constructor
     * @param raw ROM data
     */
    FileAllocationTableP2000t(const QByteArray& _data);

    /**
     * @brief Get total size of the raw data
     */
    auto size() const {
        return this->data.size();
    }

    /**
     * @brief get file metadata
     * @return vector containing file metadata
     */
    const auto& get_files() const {
        return this->files;
    }

    /**
     * @brief Build a CAS file given file index
     * @param file index
     * @return raw CAS file
     */
    QByteArray build_cas(int i);

private:
    /**
     * @brief Indexes all the files
     */
    void index_files();
};

#endif // FILEALLOCATIONTABLEP2000T_H
