#include "threadtl866.h"

ThreadTL866::ThreadTL866(QObject *parent) : QThread(parent) {

}

void ThreadTL866::run() {
    this->process = this->build_process();

    this->process->setProcessChannelMode(QProcess::MergedChannels); // combine error and standard output
    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_parse_output()));

    this->process->start();
    qDebug() << "Run process launched";
    if(this->process->waitForStarted(1000)) {
        qDebug() << "Run process started";
        if(this->process->waitForFinished(60 * 60 * 1000)) { // timeout at one hour
            qDebug() << "Run process finished";
        } else {
            qCritical() << "Run process did not finish";
        }
    } else {
        qCritical() << "Run process did not launch";
        qCritical() << this->process->errorString();
    }

    // clean up folder
    qDebug() << "Cleaning temporary folder";
    QDir dir(this->process->workingDirectory());
    dir.removeRecursively();

    // emit run complete
    emit(signal_read_done(this));
}

QProcess* ThreadTL866::build_process() {
    QString cwd = this->build_run_directory();
    qDebug() << tr("Created temporary path: ") << cwd;
    QStringList arguments = {"-p", "SST39SF040@PLCC32"};

    if(this->operation == 0) { // read
        arguments.append({"-r", "read.bin"});
    } else if(this->operation == 1) { // write
        arguments.append({"-w", "write.bin"});
    } else {
        throw std::logic_error("Unknown operation.");
    }

    QProcess* flash_process = new QProcess();
    flash_process->setProgram(cwd + "/minipro.exe");
    flash_process->setArguments(arguments);
    flash_process->setProcessChannelMode(QProcess::SeparateChannels);
    flash_process->setWorkingDirectory(cwd);

    return flash_process;
}

QString ThreadTL866::build_run_directory() {
    qDebug() << "Building run directory";
    QTemporaryDir dir;
    dir.setAutoRemove(false); // do not immediately remove
    if(dir.isValid()) {
        // copy files
        QStringList files = {
            "minipro.exe",
            "infoic.xml",
            "logicic.xml"
        };

        for(int i=0; i<files.size(); i++) {
            qDebug() << "Copying file: " << files[i];
            QFile deployable_file(":/assets/minipro/" + files[i]);
            if(!deployable_file.open(QIODevice::ReadOnly)) {
                throw std::runtime_error("Could not open emulator file from assets.");
            }
            deployable_file.copy(dir.path() + "/" + files[i]);
        }
    } else {
        throw std::runtime_error("Invalid path");
    }

    return QDir::cleanPath(dir.path());
}

void ThreadTL866::slot_parse_output() {
    const QRegularExpression regex(QStringLiteral("([0-9]+\\%)"));
    int numbytes = this->process->bytesAvailable();
    if(numbytes > 0) {
        QByteArray data = this->process->read(numbytes);
        this->output.append(data);

        auto match_iterator = regex.globalMatch(this->output, this->regexoffset);
        while(match_iterator.hasNext()) {
            auto result = match_iterator.next();
            if(!match_iterator.hasNext()) {
                qDebug() << result.captured(0);
            }
        }
    }
}
