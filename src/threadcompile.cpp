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
    QDir dir(process->workingDirectory());
    dir.removeRecursively();

    // emit compilation done
    emit(signal_compilation_done());
}

QProcess* ThreadCompile::build_process() {
    QString cwd = this->build_compilation_directory();
    qDebug() << tr("Created temporary path: ") << cwd;
    QStringList arguments = {"source.asm", "mcode.bin"};
    QProcess* blender_process = new QProcess();
    blender_process->setProgram(cwd + "/tniasm.exe");
    blender_process->setArguments(arguments);
    blender_process->setProcessChannelMode(QProcess::SeparateChannels);
    blender_process->setWorkingDirectory(cwd);

    return blender_process;
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

        // write source file
        QFile outfile(dir.path() + "/source.asm");
        if(outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&outfile);
            stream << this->source;
        }
        outfile.close();
    } else {
        throw std::runtime_error("Invalid path");
    }

    return QDir::cleanPath(dir.path());
}
