#ifndef THREADRUN_H
#define THREADRUN_H

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

class ThreadRun : public QThread {
    Q_OBJECT

public:
    enum class ProcessConfiguration {
        REGULAR = 0,
        MCODE_AS_CAS = 1,
    };

private:
    QByteArray mcode;
    QStringList output;

    ProcessConfiguration process_configuration = ProcessConfiguration::REGULAR;

public:
    ThreadRun();

    inline void set_mcode(const QByteArray& _mcode) {
        this->mcode = _mcode;
    }

    inline void set_process_configuration(ProcessConfiguration config) {
        this->process_configuration = config;
    }

    QProcess* build_process();

    void run();

private:
    QString build_run_directory();

    QProcess* launch_process();

signals:
    void signal_run_complete(void*);
};

#endif // THREADRUN_H
