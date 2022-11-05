#include "threadcompile.h"

ThreadCompile::ThreadCompile() {

}

void ThreadCompile::run() {
    QProcess* process = this->build_process();

    process->start();
    qDebug() << "Compilation process launched";
    if(process->waitForStarted(1000)) {
        qDebug() << "Compilation started";
        if(process->waitForFinished(60 * 60 * 1000)) { // timeout at one hour
            qDebug() << "Compilation finished";

            // collect output of job
            QStringList result;
            auto lines = process->readAll().split('\n');
            for(const QByteArray& line : lines) {
                result << line;
                qDebug() << line;
            }

            // read any errors
            auto error_lines = process->readAllStandardError().split('\n');
            for(const QByteArray& line : error_lines) {
                result << line;
            }

            // store output of job
            this->output = result;
        } else {
            qCritical() << "Compilation did not finish";
        }
    } else {
        qCritical() << "Compilation did not launch";
        qCritical() << process->errorString();
    }

    // read binary file as bytearray
    QFile mcodefile(process->workingDirectory() + "/mcode.bin");
    if(mcodefile.open(QIODevice::ReadOnly)) {
        this->mcode = mcodefile.readAll();
    } else {
        throw std::runtime_error("Could not open machine code file");
    }

    // clean up folder
    QDir dir(this->temppath);
    dir.removeRecursively();

    // emit compilation done
    process->waitForFinished();

    qDebug() << "Emit compilation done";
    process->close();
    emit(signal_compilation_done());
}

QProcess* ThreadCompile::build_process() {
    QString exec_path = this->build_compilation_directory();
    this->temppath = exec_path;
    qDebug() << tr("Created temporary path: ") << exec_path;
    QProcess* process = new QProcess();
    process->setProgram(exec_path + "/tniasm.exe");
    process->setProcessChannelMode(QProcess::SeparateChannels);

    QFileInfo finfo(this->sourcefile);
    process->setWorkingDirectory(finfo.absolutePath());

    QStringList arguments = {finfo.fileName(), "mcode.bin"};
    process->setArguments(arguments);

    return process;
}

QString ThreadCompile::build_compilation_directory() {
    QTemporaryDir dir;
    dir.setAutoRemove(false); // do not immediately remove
    if(dir.isValid()) {
        // copy assembler executable
        QFile assembler_executable(":/assets/assembler/tniasm.exe");
        if(!assembler_executable.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open assembler file from assets.");
        }
        assembler_executable.copy(dir.path() + "/tniasm.exe");
    } else {
        throw std::runtime_error("Invalid path");
    }

    return QDir::cleanPath(dir.path());
}
