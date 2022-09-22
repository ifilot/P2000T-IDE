#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // set main widget
    QWidget* w = new QWidget();
    this->setCentralWidget(w);

    // set main VBoxLayout
    QHBoxLayout* top_layout = new QHBoxLayout();
    w->setLayout(top_layout);

    // left screen -> text editor
    QWidget* parent_widget_text_edit = new QWidget();
    QVBoxLayout* layout_text_edit = new QVBoxLayout();
    parent_widget_text_edit->setLayout(layout_text_edit);

    // add active file label
    this->label_active_filename = new QLabel("untitled");
    layout_text_edit->addWidget(this->label_active_filename);

    // add text editor
    this->text_editor = new QTextEdit();
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    text_editor->setFont(font);
    this->highlighter = new AssemblyHighlighter(this->text_editor->document());

    // set tab stop
    const int tabStop = 4;
    QFontMetrics metrics(font);
    text_editor->setTabStopWidth(tabStop * metrics.width(' '));
    layout_text_edit->addWidget(this->text_editor);
    connect(this->text_editor, SIGNAL(textChanged()), this, SLOT(slot_editor_onchange()));

    // add text editor parent widget
    top_layout->addWidget(parent_widget_text_edit);

    // middle screen -> hex result
    this->hex_viewer = new QHexView();
    top_layout->addWidget(this->hex_viewer);

    // right screen -> log
    this->log_viewer = new QPlainTextEdit();
    top_layout->addWidget(this->log_viewer);

    this->build_menu();
    this->load_theme();
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

    // save
    QAction *action_save = new QAction(menuFile);
    action_save->setText(tr("Save"));
    action_save->setShortcuts(QKeySequence::Save);
    menuFile->addAction(action_save);
    connect(action_save, &QAction::triggered, this, &MainWindow::slot_save);

    // save as
    QAction *action_save_as = new QAction(menuFile);
    action_save_as->setText(tr("Save as"));
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    menuFile->addAction(action_save_as);
    connect(action_save_as, &QAction::triggered, this, &MainWindow::slot_save_as);

    // Compile
    QAction *action_compile = new QAction(menuBuild);
    action_compile->setText(tr("Compile"));
    menuBuild->addAction(action_compile);
    action_compile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    connect(action_compile, &QAction::triggered, this, &MainWindow::slot_compile);

    // Run
    QAction *action_run = new QAction(menuBuild);
    action_run->setText(tr("Run"));
    menuBuild->addAction(action_run);
    action_run->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(action_run, &QAction::triggered, this, &MainWindow::slot_run);

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
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Assembly source files (*.asm)"));

    // do nothing if user has cancelled
    if(filename.isEmpty()) {
        return;
    }

    // write source file
    QFile sourcefile(filename);
    if(sourcefile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray contents = sourcefile.readAll();
        this->text_editor->setPlainText(contents);
    }
    sourcefile.close();

    this->label_active_filename->setText(filename);
}

/**
 * @brief save a file
 */
void MainWindow::slot_save() {
    // do not save if there are no changed
    if(!this->label_active_filename->text().endsWith('*')) {
        return;
    }

    // ask user where to save file
    if(this->label_active_filename->text().startsWith("untitled")) {
        return this->slot_save_as();
    }

    // remove asterisk
    QString url = this->label_active_filename->text();
    url.resize(this->label_active_filename->text().size()-1);
    QFile sourcefile(url);
    if(sourcefile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&sourcefile);
        stream << this->text_editor->toPlainText();
    }
    sourcefile.close();
    qDebug() << "Saved sourcecode to " << url;

    // rewrite label
    this->label_active_filename->setText(url);
}

/**
 * @brief save a file
 */
void MainWindow::slot_save_as() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Assembly source files (*.asm)"));

    // do nothing if user has cancelled
    if(filename.isEmpty()) {
        return;
    }

    QFile sourcefile(filename);
    if(sourcefile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&sourcefile);
        stream << this->text_editor->toPlainText();
    }
    qDebug() << "Saved sourcecode to new file " << filename;

    // rewrite label
    this->label_active_filename->setText(filename);
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
 * @brief compile file
 */
void MainWindow::slot_run() {
    ThreadRun* runthread = new ThreadRun();
    runthread->set_mcode(this->compile_job->get_mcode());
    connect(runthread, SIGNAL(signal_run_complete(void*)), this, SLOT(slot_run_complete(void*)));
    runthread->start();
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

/**
 * @brief void slot_run_complete
 */
void MainWindow::slot_run_complete(void* pointer) {
    qDebug() << "Cleaning up run object";
    delete static_cast<ThreadRun*>(pointer);
}

MainWindow::~MainWindow() {
}

/**
 * @brief      Loads a theme.
 */
void MainWindow::load_theme() {
    // load theme
    QFile f(":/assets/themes/darkorange/darkorange.qss");
    if (!f.exists())   {
        throw std::runtime_error("Cannot open theme file.");
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

/**
 * @brief slot when text editor has changed
 */
void MainWindow::slot_editor_onchange() {
    if(!this->label_active_filename->text().endsWith('*')) {
        this->label_active_filename->setText(this->label_active_filename->text() + '*');
    }
}
