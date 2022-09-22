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

#include "qhexview.h"
#include "config.h"
#include "threadcompile.h"
#include "threadrun.h"
#include "assemblyhighlighter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QTextEdit* text_editor;
    QLabel* label_active_filename;
    QPlainTextEdit* log_viewer;
    QHexView* hex_viewer;
    std::unique_ptr<ThreadCompile> compile_job;
    AssemblyHighlighter* highlighter;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void build_menu();

    void load_theme();

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
};
#endif // MAINWINDOW_H
