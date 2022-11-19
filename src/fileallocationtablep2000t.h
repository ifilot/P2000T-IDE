#ifndef FILEALLOCATIONTABLEP2000T_H
#define FILEALLOCATIONTABLEP2000T_H

#include <QByteArray>
#include <qDebug>

class FileAllocationTableP2000t {

private:
    QByteArray data;

public:
    FileAllocationTableP2000t(const QByteArray& _data);

private:
    bool valid_metaheaders();
};

#endif // FILEALLOCATIONTABLEP2000T_H
