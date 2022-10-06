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
    AssemblyHighlighter* highlighter;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void build_menu();

    void load_theme();

    void add_groupbox_and_widget(const QString& name,
                                 QLayout* layout,
                                 QWidget* widget);

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
    void slot_save_machine_code();

    /**
     * @brief slot_load_example_file
     */
    void slot_load_example_file();

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
};
#endif // MAINWINDOW_H
