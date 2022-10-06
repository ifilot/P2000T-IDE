#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // set main widget
    QWidget* w = new QWidget();
    this->setCentralWidget(w);

    // set main VBoxLayout
    QHBoxLayout* top_layout = new QHBoxLayout();
    top_layout->setMargin(5);
    w->setLayout(top_layout);

    //
    // left screen -> text editor
    //
    QGroupBox* parent_widget_text_edit = new QGroupBox("Source code editor");
    parent_widget_text_edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout* layout_text_edit = new QVBoxLayout();
    parent_widget_text_edit->setLayout(layout_text_edit);

    // add active file label
    this->label_active_filename = new QLabel("untitled");
    layout_text_edit->addWidget(this->label_active_filename);

    // add text editor
    this->code_editor = new CodeEditor();
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    this->code_editor->setFont(font);
    this->highlighter = new AssemblyHighlighter(this->code_editor->document());

    // set tab stop
    const int tabStop = 4;
    QFontMetrics metrics(font);
    this->code_editor->setTabStopWidth(tabStop * metrics.width(' '));
    layout_text_edit->addWidget(this->code_editor);
    connect(this->code_editor, SIGNAL(textChanged()), this, SLOT(slot_editor_onchange()));

    // add text editor parent widget
    top_layout->addWidget(parent_widget_text_edit);

    //
    // middle screen -> hex result
    ///
    QGroupBox* hex_viewer_container = new QGroupBox("Machine code viewer");
    hex_viewer_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout* layout_hexviewer = new QVBoxLayout();
    hex_viewer_container->setLayout(layout_hexviewer);
    this->hex_viewer = new QHexView();
    layout_hexviewer->addWidget(this->hex_viewer);
    top_layout->addWidget(hex_viewer_container);

    //
    // right screen -> log and upload interface
    //
    QWidget* widget_right_screen_container = new QWidget();
    QVBoxLayout* widget_right_screen_layout = new QVBoxLayout();
    widget_right_screen_container->setLayout(widget_right_screen_layout);

    // logviewer
    this->log_viewer = new QPlainTextEdit();
    this->add_groupbox_and_widget("Log", widget_right_screen_layout, this->log_viewer);
    top_layout->addWidget(widget_right_screen_container);

    // serial interface
    this->serial_widget = new SerialWidget();
    this->add_groupbox_and_widget("P2000T Cartridge Interface", widget_right_screen_layout, this->serial_widget);
    top_layout->addWidget(widget_right_screen_container);

    // set statusbar
    statusBar()->showMessage(tr("Ready"));

    // connect messages to statusbar
    connect(this->serial_widget, SIGNAL(signal_emit_statusbar_message(const QString&)), statusBar(), SLOT(showMessage(const QString&)));
    connect(this->serial_widget, SIGNAL(signal_data_read()), this, SLOT(slot_serial_parse_data()));
    connect(this->serial_widget, SIGNAL(signal_get_data()), this, SLOT(slot_serial_assert_data()));

    this->build_menu();

    // set Window icon and title
    this->setWindowTitle(tr(PROGRAM_NAME) + " " + tr(PROGRAM_VERSION));
    this->setWindowIcon(QIcon(":/assets/images/p2000t-ide.ico"));
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

    // save machine code
    menuFile->addSeparator();
    QAction *action_save_machine_code = new QAction(menuFile);
    action_save_machine_code->setText(tr("Save machine code"));
    action_save_machine_code->setShortcuts(QKeySequence::SaveAs);
    menuFile->addAction(action_save_machine_code);
    connect(action_save_machine_code, &QAction::triggered, this, &MainWindow::slot_save_machine_code);

    // add code examples
    menuFile->addSeparator();
    QStringList sample_files = {"helloworld.asm"};
    QMenu *menu_samples = menuFile->addMenu(tr("&Examples"));
    for(int i=0; i<sample_files.size(); i++) {
        QAction *action_load_example = new QAction(sample_files[i]);
        action_load_example->setText(sample_files[i]);
        menu_samples->addAction(action_load_example);
        action_load_example->setData(QVariant(sample_files[i]));
        connect(action_load_example, &QAction::triggered, this, &MainWindow::slot_load_example_file);
    }

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
    menuFile->addSeparator();
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
        this->code_editor->setPlainText(contents);
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
        stream << this->code_editor->toPlainText();
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
        stream << this->code_editor->toPlainText();
    }
    qDebug() << "Saved sourcecode to new file " << filename;

    // rewrite label
    this->label_active_filename->setText(filename);
}

/**
 * @brief save machine code
 */
void MainWindow::slot_save_machine_code() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Assembly source files (*.bin)"));

    // do nothing if user has cancelled
    if(filename.isEmpty()) {
        return;
    }

    QFile sourcefile(filename);
    if(sourcefile.open(QIODevice::WriteOnly)) {
        sourcefile.write(this->hex_viewer->get_data());
    }
    qDebug() << "Saved sourcecode to new file " << filename;

    // rewrite label
    this->label_active_filename->setText(filename);
}

/**
 * @brief slot_load_example_file
 */
void MainWindow::slot_load_example_file() {
    // get file
    QAction* act = qobject_cast<QAction *>(sender());
    if (act != 0) {
        QVariant data = act->data();
        QString filename = data.toString();
        qDebug() << "Loading: " << filename;

        // set source in code editor
        QFile source_file(":/assets/code/" + filename);
        if(source_file.exists()) {
            if(source_file.open(QIODevice::ReadOnly)) {
                this->code_editor->setPlainText(source_file.readAll());
            }
        }
     }
}

/**
 * @brief compile file
 */
void MainWindow::slot_compile() {
    // always save before compiling
    this->slot_save();

    QString source = this->code_editor->toPlainText();
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
                        ".\n\nAuthor:\nIvo Filot <ivo@ivofilot.nl>\n\n"
                        PROGRAM_NAME " is licensed under the GPLv3 license.\n\n"
                        PROGRAM_NAME " is dynamically linked to Qt, which is licensed under LGPLv3.\n");
    message_box.setIcon(QMessageBox::Information);
    message_box.setWindowTitle("About " + tr(PROGRAM_NAME));
    message_box.setWindowIcon(QIcon(":/assets/images/p2000t-ide.ico"));
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
 * @brief Convenience function to add GroupBox and widget
 */
void MainWindow::add_groupbox_and_widget(const QString& name, QLayout* layout, QWidget* widget) {
    QGroupBox* groupbox_container = new QGroupBox(name);
    QVBoxLayout* groupbox_layout = new QVBoxLayout();
    groupbox_container->setLayout(groupbox_layout);
    groupbox_layout->addWidget(widget);
    layout->addWidget(groupbox_container);
}

/**
 * @brief slot when text editor has changed
 */
void MainWindow::slot_editor_onchange() {
    if(!this->label_active_filename->text().endsWith('*')) {
        this->label_active_filename->setText(this->label_active_filename->text() + '*');
    }
}

/**
 * @brief Get data from SerialWidget class and parse to hex editor
 */
void MainWindow::slot_serial_parse_data() {
    auto data = this->serial_widget->get_data();
    QHexView::DataStorageArray* mcode = new QHexView::DataStorageArray(data);
    this->hex_viewer->setData(mcode);
    this->hex_viewer->viewport()->update();
}

/**
 * @brief Parse data from Hex Editor to SerialWidget class
 */
void MainWindow::slot_serial_assert_data() {
    auto data = this->hex_viewer->get_data();
    this->serial_widget->set_flash_data(data);
}
