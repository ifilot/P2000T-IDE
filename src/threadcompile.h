#ifndef THREADCOMPILE_H
#define THREADCOMPILE_H

#include <QThread>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <QDir>
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QByteArray>

class ThreadCompile : public QThread {
    Q_OBJECT

private:
    QString sourcefile;
    QStringList output;
    QByteArray mcode;
    QString temppath;

public:
    ThreadCompile();

    inline void set_source_file(const QString& src) {
        this->sourcefile = src;
    }

    inline const QStringList& get_output() const {
        return this->output;
    }

    inline const auto& get_mcode() const {
        return this->mcode;
    }

    void run();

private:
    QString build_compilation_directory();

    QProcess* build_process();

signals:
    void signal_compilation_done();
};

#endif // THREADCOMPILE_H
