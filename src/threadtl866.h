#ifndef THREADTL866_H
#define THREADTL866_H

#include <QThread>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTemporaryDir>
#include <QString>
#include <QProcess>
#include <QRegularExpression>

class ThreadTL866 : public QThread
{
    Q_OBJECT

private:
    int operation = -1; // -1 unset, 0 read, 1 write
    QByteArray data;
    QByteArray output;
    QProcess* process = nullptr;

    const auto& get_output() const {
        return this->output;
    }

    const auto& get_data() const {
        return this->data;
    }

    int regexoffset = 0;

public:
    explicit ThreadTL866(QObject *parent = nullptr);

    inline void set_operation(int _operation) {
        this->operation = _operation;
    }

    void run();

private:
    QString build_run_directory();

    QProcess* build_process();

signals:
    void signal_read_done(void*);

private slots:
    void slot_parse_output();
};

#endif // THREADTL866_H
