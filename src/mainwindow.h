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

#include "qhexview.h"
#include "config.h"
#include "threadcompile.h"
#include "threadrun.h"
#include "assemblyhighlighter.h"
#include "serialwidget.h"
#include "codeeditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    CodeEditor* code_editor;
    QLabel* label_active_filename;
    QPlainTextEdit* log_viewer;
    QHexView* hex_viewer;
    SerialWidget* serial_widget;
    std::unique_ptr<ThreadCompile> compile_job;
    std::mutex compile_mutex;
    AssemblyHighlighter* highlighter;
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
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
