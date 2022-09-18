#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // set main widget
    QWidget* w = new QWidget();
    this->setCentralWidget(w);

    // set main VBoxLayout
    QHBoxLayout* top_layout = new QHBoxLayout();
    w->setLayout(top_layout);

    // left screen -> text editor
    this->text_editor = new QTextEdit();
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    text_editor->setFont(font);

    // set tab stop
    const int tabStop = 4;
    QFontMetrics metrics(font);
    text_editor->setTabStopWidth(tabStop * metrics.width(' '));
    top_layout->addWidget(this->text_editor);

    // middle screen -> hex result
    this->hex_viewer = new QHexView();
    top_layout->addWidget(this->hex_viewer);

    // right screen -> log
    this->log_viewer = new QPlainTextEdit();
    top_layout->addWidget(this->log_viewer);

    this->build_menu();
}

void MainWindow::build_menu() {
    // create menu bar
    QMenuBar *menuBar = new QMenuBar;

    // add drop-down menus
    QMenu *menuFile = menuBar->addMenu(tr("&File"));
    QMenu *menuBuild = menuBar->addMenu(tr("&Build"));
    QMenu *menuHelp = menuBar->addMenu(tr("&Help"));

    // open
    QAction *action_open = new QAction(menuFile);
    action_open->setText(tr("Open"));
    action_open->setShortcuts(QKeySequence::Open);
    menuFile->addAction(action_open);
    connect(action_open, &QAction::triggered, this, &MainWindow::slot_open);

    // Compile
    QAction *action_compile = new QAction(menuBuild);
    action_compile->setText(tr("Compile"));
    menuBuild->addAction(action_compile);
    connect(action_compile, &QAction::triggered, this, &MainWindow::slot_compile);

    // quit
    QAction *action_quit = new QAction(menuFile);
    action_quit->setText(tr("Quit"));
    action_quit->setShortcuts(QKeySequence::Quit);
    menuFile->addAction(action_quit);
    connect(action_quit, &QAction::triggered, this, &MainWindow::slot_exit);

    // about
    QAction *action_about = new QAction(menuHelp);
    action_about->setText(tr("About"));
    menuHelp->addAction(action_about);
    action_about ->setShortcut(QKeySequence::WhatsThis);
    connect(action_about, &QAction::triggered, this, &MainWindow::slot_about);

    // build menu
    setMenuBar(menuBar);
}

/**
 * @brief open a file
 */
void MainWindow::slot_open() {

}

/**
 * @brief compile file
 */
void MainWindow::slot_compile() {
    QString source = this->text_editor->toPlainText();
    this->compile_job = std::make_unique<ThreadCompile>();
    compile_job->set_source(source);
    connect(compile_job.get(), SIGNAL(signal_compilation_done()), this, SLOT(slot_compilation_done()));
    compile_job->start();
}

/**
 * @brief exit program
 */
void MainWindow::slot_exit() {
    QApplication::quit();
}

/**
 * @brief about window
 */
void MainWindow::slot_about() {
    QMessageBox message_box;
    //message_box.setStyleSheet("QLabel{min-width: 250px; font-weight: normal;}");
    message_box.setText(PROGRAM_NAME
                        " version "
                        PROGRAM_VERSION
                        ".\n\nAuthor:\nIvo Filot <i.a.w.filot@tue.nl>\n\n"
                        PROGRAM_NAME " is licensed under the GPLv3 license.\n\n"
                        PROGRAM_NAME " is dynamically linked to Qt, which is licensed under LGPLv3.\n");
    message_box.setIcon(QMessageBox::Information);
    message_box.setWindowTitle("About " + tr(PROGRAM_NAME));
    message_box.setWindowIcon(QIcon(":/assets/icons/saucepan.ico"));
    message_box.exec();
}

/**
 * @brief about window
 */
void MainWindow::slot_compilation_done() {
    // build log
    qDebug() << "Receive compilation done";
    this->log_viewer->setPlainText(this->compile_job->get_output().join(""));

    // show hexcode
    QHexView::DataStorageArray* mcode = new QHexView::DataStorageArray(this->compile_job->get_mcode());
    this->hex_viewer->setData(mcode);
    this->hex_viewer->viewport()->update();
}

MainWindow::~MainWindow() {
}

