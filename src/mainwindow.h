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

#include "qhexview.h"
#include "config.h"
#include "threadcompile.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QTextEdit* text_editor;
    QPlainTextEdit* log_viewer;
    QHexView* hex_viewer;
    std::unique_ptr<ThreadCompile> compile_job;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void build_menu();

private slots:
    /**
     * @brief open a file
     */
    void slot_open();

    /**
     * @brief compile a file
     */
    void slot_compile();

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
};
#endif // MAINWINDOW_H
