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

    // add search widget
    this->search_widget = new SearchWidget();
    layout_text_edit->addWidget(this->search_widget);
    connect(this->search_widget, SIGNAL(search()), this, SLOT(slot_search_code()));
    connect(this->search_widget, SIGNAL(search_done()), this->code_editor, SLOT(setFocus()));

    //
    // middle screen -> hex result
    ///
    QGroupBox* hex_viewer_container = new QGroupBox("Machine code viewer");
    hex_viewer_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout* layout_hexviewer = new QVBoxLayout();
    hex_viewer_container->setLayout(layout_hexviewer);
    this->hex_viewer = new QHexView();

    // hex info
    QWidget* widget_hexinfo = new QWidget();
    QHBoxLayout* layout_hexinfo = new QHBoxLayout();
    widget_hexinfo->setLayout(layout_hexinfo);
    this->label_machine_code_data = new QLabel("");
    this->progressbar_storage = new QProgressBar();
    this->progressbar_storage->setMinimum(0);
    this->progressbar_storage->setMaximum(16*1024);
    this->progressbar_storage->setVisible(false);
    this->progressbar_storage->setStyleSheet(" QProgressBar { border: 1px solid #292824; border-radius: 3px; text-align: center; height: 5px;} QProgressBar::chunk {background-color: #1fad83; width: 1px;}");
    layout_hexinfo->addWidget(this->label_machine_code_data);
    layout_hexinfo->addWidget(this->progressbar_storage);

    // add widgets to middle level container
    layout_hexviewer->addWidget(widget_hexinfo);
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

    // tl866 interface widget
    this->tl866_widget = new TL866Widget();
    this->add_groupbox_and_widget("TL866II+ Interface", widget_right_screen_layout, this->tl866_widget);
    top_layout->addWidget(widget_right_screen_container);

    // set statusbar
    statusBar()->showMessage(tr("Ready"));

    // connect signals and slots for serial interface
    connect(this->serial_widget, SIGNAL(signal_emit_statusbar_message(const QString&)), statusBar(), SLOT(showMessage(const QString&)));
    connect(this->serial_widget, SIGNAL(signal_data_read()), this, SLOT(slot_serial_parse_data()));
    connect(this->serial_widget, SIGNAL(signal_get_data()), this, SLOT(slot_serial_assert_data()));

    // connect signals and slots for TL866 widget
    connect(this->tl866_widget, SIGNAL(signal_data_read()), this, SLOT(slot_tl866_parse_data()));
    connect(this->tl866_widget, SIGNAL(signal_get_data()), this, SLOT(slot_tl866_assert_data()));
    connect(this->tl866_widget, SIGNAL(signal_log_read()), this, SLOT(slot_tl866_parse_log()));

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
    QMenu *menuEdit = menuBar->addMenu(tr("&Edit"));
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

    // load machine code
    menuFile->addSeparator();
    QAction *action_load_machine_code = new QAction(menuFile);
    action_load_machine_code->setText(tr("Load binary"));
    menuFile->addAction(action_load_machine_code);
    connect(action_load_machine_code, &QAction::triggered, this, &MainWindow::slot_load_machine_code);

    // save machine code
    QAction *action_save_machine_code = new QAction(menuFile);
    action_save_machine_code->setText(tr("Save binary"));
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
        action_load_example->setData(QVariant(":/assets/code/" + sample_files[i]));
        connect(action_load_example, &QAction::triggered, this, &MainWindow::slot_load_file);
    }

    // add code recent files
    menuFile->addSeparator();
    QSettings settings;
    QStringList recent_files = settings.value(this->RECENT_FILES_KEYWORD).toStringList();
    QMenu *menu_recent_files = menuFile->addMenu(tr("&Recent Files"));
    for(int i=0; i<this->MAX_RECENT_FILES; i++) {
        QAction* action = new QAction("File");
        this->recent_file_action_list.append(action);
        action->setVisible(false);
        menu_recent_files->addAction(action);
        connect(action, &QAction::triggered, this, &MainWindow::slot_load_file);
    }

    /*
     * Edit menu
     */

    // find
    QAction *action_search = new QAction(menuEdit);
    action_search->setText(tr("Search"));
    action_search->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    menuEdit->addAction(action_search);
    connect(action_search, SIGNAL(triggered()), this->search_widget, SLOT(show_search_widget()));

    /*
     * Build menu
     */

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

    // update actionlist for files
    this->update_recent_action_filelist();
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
        sourcefile.close();
        this->update_recent_files_list(filename);
    }

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
        sourcefile.close();
        update_recent_files_list(url);
    }

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
        sourcefile.close();

        update_recent_files_list(filename);
    }
    qDebug() << "Saved sourcecode to new file " << filename;

    // rewrite label
    this->label_active_filename->setText(filename);
}

/**
 * @brief save machine code
 */
void MainWindow::slot_load_machine_code() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Assembly source files (*.bin)"));

    // do nothing if user has cancelled
    if(filename.isEmpty()) {
        return;
    }

    QFile sourcefile(filename);
    if(sourcefile.open(QIODevice::ReadOnly)) {
        this->hex_viewer->setData(new QHexView::DataStorageArray(sourcefile.readAll()));
        this->hex_viewer->viewport()->update();
    }
    qDebug() << "Load sourcecode from file: " << filename;
}

/**
 * @brief save machine code
 */
void MainWindow::slot_save_machine_code() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
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
}

/**
 * @brief slot_load_file
 */
void MainWindow::slot_load_file() {
    // get file
    QAction* act = qobject_cast<QAction *>(sender());
    if (act != 0) {
        QVariant data = act->data();
        QString filename = data.toString();
        qDebug() << "Loading: " << filename;

        // set source in code editor
        QFile source_file(filename);
        if(source_file.exists()) {
            if(source_file.open(QIODevice::ReadOnly)) {
                this->code_editor->setPlainText(source_file.readAll());
            }
            source_file.close();

            if(!filename.startsWith(":/assets")) {
                this->update_recent_files_list(filename);
                this->label_active_filename->setText(filename);
            }

        }
     }
}

/**
 * @brief compile file
 */
void MainWindow::slot_compile() {
    // try to lock, else wait
    unsigned int ctr=0;
    qDebug() << "Try to lock mutex";
    while(!this->compile_mutex.try_lock()){
        qDebug() << "Waiting for lock: " << ctr;

        if(ctr > 10) { // exit as some point
            return;
        }

        ctr++;
    }

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
    qDebug() << "Running code...";
    if(this->hex_viewer->get_data().size() == 0) {
        qDebug() << "Not yet compiled. Cancelling.";
        return;
    }

    ThreadRun* runthread = new ThreadRun();
    runthread->set_mcode(this->hex_viewer->get_data());
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
                        PROGRAM_NAME " is licensed under the GPLv3 license.\n"
                        PROGRAM_NAME " is dynamically linked to Qt, which is licensed under LGPLv3.\n"
                        "The source code of this program can be found at: https://github.com/ifilot/P2000T-IDE\n\n"
                        "This software comes bundled with the following vendor packages:\n"
                        "tniASM, Minipro, and M2000.\n\n"
                        "tniASM Macro Assembler, which is developed by Patriek Lesparre."
                        "More information can be found at: http://tniasm.tni.nl/\n\n"
                        "Minipro is an open source program for controlling the MiniPRO TL866xx series "
                        "of chip programmers. More information can be found at: https://gitlab.com/DavidGriffith/minipro/\n\n"
                        "M2000 is an all-in-one P2000T emulator developed by Marcel de Kogel. More information"
                        "on this emulator can be found at: http://www.komkon.org/~dekogel/m2000.html");
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

    // update machine code label
    this->label_machine_code_data->setText(tr("%1 bytes / 16384 bytes").arg(mcode->size()));
    this->progressbar_storage->setVisible(true);
    this->progressbar_storage->setValue(mcode->size());

    // delete compilation object and unlock mutex
    this->compile_mutex.unlock();
    this->compile_job.reset();
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
 * @brief update_recent_files_list
 * @param filename
 */
void MainWindow::update_recent_files_list(const QString& filename) {
    QSettings settings;

    // verify if settings object contains a recent_files variable
    if(!settings.contains(this->RECENT_FILES_KEYWORD)) {
        settings.setValue(this->RECENT_FILES_KEYWORD, QVariant(QStringList()));
    }

    // load recent files paths
    QStringList recent_files_paths = settings.value(this->RECENT_FILES_KEYWORD).toStringList();

    // check whether the item already exists, if so, ensure that the
    // recent file is placed at the top
    if(recent_files_paths.contains(filename)) {
        QStringList new_recent_file_list;
        new_recent_file_list.append(filename);
        for(int i=0; i<recent_files_paths.size(); i++) {
            if(recent_files_paths[i] != filename) {
                new_recent_file_list.append(recent_files_paths[i]);
            }
        }
        settings.setValue(this->RECENT_FILES_KEYWORD, QVariant(recent_files_paths));
        this->update_recent_action_filelist();
        return;
    }

    // if the file is not yet in the list, add if to the list and
    // remove any old items if the list is already at its max capacity
    if(recent_files_paths.size() >= this->MAX_RECENT_FILES) {
        for(int i=0; i<7; i++) {
            recent_files_paths[i] = recent_files_paths[i+1];
        }
        recent_files_paths[7] = filename;
    } else {
        recent_files_paths.append(filename);
    }

    qDebug() << "Updating recent files";
    for(int i=0; i<recent_files_paths.size(); i++) {
        qDebug() << i << " " << recent_files_paths[i];
    }

    settings.setValue(this->RECENT_FILES_KEYWORD, QVariant(recent_files_paths));
    this->update_recent_action_filelist();
}

/**
 * @brief Update the recent files menu
 */
void MainWindow::update_recent_action_filelist() {
    QSettings settings;

    // verify if settings object contains a recent_files variable
    if(!settings.contains(this->RECENT_FILES_KEYWORD)) {
        settings.setValue(this->RECENT_FILES_KEYWORD, QVariant(QStringList()));
    }

    QStringList recent_files_paths = settings.value(this->RECENT_FILES_KEYWORD).toStringList();
    for(int i=0; i<this->MAX_RECENT_FILES; i++) {
        if(i < recent_files_paths.size()) {
            QFileInfo file_info(recent_files_paths[i]);
            if(file_info.exists() && file_info.completeSuffix() != "bin") { // ignore binary files
                this->recent_file_action_list[i]->setVisible(true);
                this->recent_file_action_list[i]->setData(recent_files_paths[i]);
                this->recent_file_action_list[i]->setText(file_info.fileName());
            } else {
                this->recent_file_action_list[i]->setVisible(false);
            }
        } else {
            this->recent_file_action_list[i]->setVisible(false);
        }
    }
}

/**
 * @brief write_settings
 */
void MainWindow::write_settings() {
    // do something to settings
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
}

/**
 * @brief Parse data from Hex Editor to SerialWidget class
 */
void MainWindow::slot_serial_assert_data() {
    auto data = this->hex_viewer->get_data();
    this->serial_widget->set_flash_data(data);
}

/**
 * @brief Get data from SerialWidget class and parse to hex editor
 */
void MainWindow::slot_tl866_parse_data() {
    auto data = this->tl866_widget->get_data();
    QHexView::DataStorageArray* mcode = new QHexView::DataStorageArray(data);
    this->hex_viewer->setData(mcode);
}

/**
 * @brief Parse data from Hex Editor to SerialWidget class
 */
void MainWindow::slot_tl866_assert_data() {
    auto data = this->hex_viewer->get_data();
    this->tl866_widget->set_flash_data(data);
}

/**
 * @brief Parse log from TL866 widget to log object
 */
void MainWindow::slot_tl866_parse_log() {
    auto log = this->tl866_widget->get_log_data();

    // clean log
    QString logstring(log);
    logstring.remove("[K");

    // send log to log object
    this->log_viewer->setPlainText(logstring);
}

/**
 * @brief slot_search_code
 */
void MainWindow::slot_search_code() {
    QString word = this->search_widget->get_line_edit_ptr()->text();
    this->code_editor->search(word);
}

/**
 * @brief Close event
 */
void MainWindow::closeEvent(QCloseEvent *event){
    this->write_settings();
    QMainWindow::closeEvent(event);
}
