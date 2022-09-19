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

private:
    QByteArray mcode;

    QStringList output;

public:
    ThreadRun();

    inline void set_mcode(const QByteArray& _mcode) {
        this->mcode = _mcode;
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
