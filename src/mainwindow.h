#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QStatusBar>
#include <QGroupBox>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QTextCursor>

#include "qhexview.h"
#include "config.h"
#include "threadcompile.h"
#include "threadrun.h"
#include "assemblyhighlighter.h"
#include "serialwidget.h"
#include "codeeditor.h"
#include "tl866widget.h"
#include "searchwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    // code editor
    QLabel* label_active_filename;
    CodeEditor* code_editor;
    SearchWidget* search_widget;
    AssemblyHighlighter* highlighter;

    // hex widget / info
    QHexView* hex_viewer;
    QLabel* label_machine_code_data;
    QProgressBar* progressbar_storage;

    // log
    QPlainTextEdit* log_viewer;

    // serial interface
    SerialWidget* serial_widget;

    // TL866 interface
    TL866Widget* tl866_widget;

    // other
    std::mutex compile_mutex;
    std::unique_ptr<ThreadCompile> compile_job;
    QList<QAction*> recent_file_action_list;

    const unsigned int MAX_RECENT_FILES = 8;
    const QString RECENT_FILES_KEYWORD = "recent_files";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void build_menu();

    void load_theme();

    void add_groupbox_and_widget(const QString& name,
                                 QLayout* layout,
                                 QWidget* widget);

    /**
     * @brief update_recent_files_list
     * @param filename
     */
    void update_recent_files_list(const QString& filename);

    /**
     * @brief Update the recent files menu
     */
    void update_recent_action_filelist();

    /**
     * @brief write_settings
     */
    void write_settings();

private slots:
    /**
     * @brief open a file
     */
    void slot_open();

    /**
     * @brief save a file
     */
    void slot_save();

    /**
     * @brief save a file
     */
    void slot_save_as();

    /**
     * @brief save machine code
     */
    void slot_load_machine_code();

    /**
     * @brief save machine code
     */
    void slot_save_machine_code();

    /**
     * @brief slot_load_example_file
     */
    void slot_load_file();

    /**
     * @brief compile a file
     */
    void slot_compile();

    /**
     * @brief compile a file
     */
    void slot_run();

    /**
     * @brief exit program
     */
    void slot_exit();

    /**
     * @brief about window
     */
    void slot_about();

    /**
     * @brief void slot_compilation_done
     */
    void slot_compilation_done();

    /**
     * @brief void slot_compilation_done
     */
    void slot_run_complete(void*);

    /**
     * @brief slot when text editor has changed
     */
    void slot_editor_onchange();

    /**
     * @brief Get data from SerialWidget class and parse to hex editor
     */
    void slot_serial_parse_data();

    /**
     * @brief Parse data from Hex Editor to SerialWidget class
     */
    void slot_serial_assert_data();

    /**
     * @brief Get data from SerialWidget class and parse to hex editor
     */
    void slot_tl866_parse_data();

    /**
     * @brief Parse data from Hex Editor to SerialWidget class
     */
    void slot_tl866_assert_data();

    /**
     * @brief Parse log from TL866 widget to log object
     */
    void slot_tl866_parse_log();

    /**
     * @brief slot_search_code
     */
    void slot_search_code();

    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
