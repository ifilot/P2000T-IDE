#include "threadrun.h"

ThreadRun::ThreadRun() {

}

void ThreadRun::run() {
    QProcess* process = this->build_process();

    process->start();
    qDebug() << "Run process launched";
    if(process->waitForStarted(1000)) {
        qDebug() << "Run process started";
        if(process->waitForFinished(60 * 60 * 1000)) { // timeout at one hour
            qDebug() << "Run process finished";

            // collect output of job
            QStringList result;
            auto lines = process->readAll().split('\n');
            for(const QByteArray& line : lines) {
                result << line;
                //qDebug() << line;
            }

            // read any errors
            auto error_lines = process->readAllStandardError().split('\n');
            for(const QByteArray& line : error_lines) {
                result << line;
            }

            // store output of job
            this->output = result;
        } else {
            qCritical() << "Run process did not finish";
        }
    } else {
        qCritical() << "Run process did not launch";
        qCritical() << process->errorString();
    }

    // clean up folder
    qDebug() << "Cleaning temporary folder";
    QDir dir(process->workingDirectory());
    dir.removeRecursively();

    // emit run complete
    emit(signal_run_complete(this));
}

QProcess* ThreadRun::build_process() {
    QString cwd = this->build_run_directory();
    qDebug() << tr("Created temporary path: ") << cwd;
    QStringList arguments = {};
    QProcess* blender_process = new QProcess();
    blender_process->setProgram(cwd + "/m2000.exe");
    blender_process->setArguments(arguments);
    blender_process->setProcessChannelMode(QProcess::SeparateChannels);
    blender_process->setWorkingDirectory(cwd);

    return blender_process;
}

QString ThreadRun::build_run_directory() {
    qDebug() << "Building run directory";
    QTemporaryDir dir;
    dir.setAutoRemove(false); // do not immediately remove
    if(dir.isValid()) {
        // copy files
        QStringList files = {
            "Default.fnt",
            "fontc.exe",
            "libjpeg-8.dll",
            "libpng16-16.dll",
            "libwebp-7.dll",
            "libwinpthread-1.dll",
            "m2000.exe",
            "m2000.txt",
            "P2000.cas",
            "p2000rom.bin",
            "zlib1.dll"
        };

        for(int i=0; i<files.size(); i++) {
            qDebug() << "Copying file: " << files[i];
            QFile assembler_executable(":/assets/emulator/" + files[i]);
            if(!assembler_executable.open(QIODevice::ReadOnly)) {
                throw std::runtime_error("Could not open emulator file from assets.");
            }
            assembler_executable.copy(dir.path() + "/" + files[i]);
        }

        // write binary file
        QFile outfile(dir.path() + "/BASIC.bin");
        if(outfile.open(QIODevice::WriteOnly)) {
            outfile.write(this->mcode);
        }
        outfile.close();
    } else {
        throw std::runtime_error("Invalid path");
    }

    return QDir::cleanPath(dir.path());
}
