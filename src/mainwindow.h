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
#include <QTabWidget>

#include "qhexview.h"
#include "config.h"
#include "threadcompile.h"
#include "threadrun.h"
#include "assemblyhighlighter.h"
#include "serialwidget.h"
#include "codeeditor.h"
#include "tl866widget.h"
#include "searchwidget.h"
#include "fileallocationtablep2000t.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    // code editor
    QVector<CodeEditor*> code_editors;
    QTabWidget* code_tabs;
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

    CodeEditor* new_code_editor();

    CodeEditor* get_active_code_editor();

    void delete_code_editor(CodeEditor*);

private slots:
    /**
     * @brief create a new file
     */
    void slot_new();

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
     * @brief Load a file from the menu listing
     *
     * Load a file from the menu and create a new code editor tab
     * for it. If such a tab already exists, give that tab focus.
     */
    void slot_load_file();

    /**
     * @brief Close the file that is opened
     */
    void slot_close_file(int tab_id);

    /**
     * @brief Compile active file
     *
     * Compile (assemble) the source code in the currently active Code Editor. Perform
     * this compilation in the folder the source code resides in.
     */
    void slot_compile();

    /**
     * @brief compile a file
     */
    void slot_run();

    /**
     * @brief slot_run_mcode_as_cas
     */
    void slot_run_mcode_as_cas();

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

    /**
     * @brief Go to next tab
     */
    void slot_toggletab_forward();

    /**
     * @brief Go to previous tab
     */
    void slot_toggletab_backward();

    /**
     * @brief Close tab
     */
    void slot_close_tab();

    /**
     * @brief Analyze hexcode as if it is FAT P2000T FAT
     */
    void slot_analyze_fat_p2000t();
};
#endif // MAINWINDOW_H
