#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QDebug>
#include <QShortCut>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

private:
    QString filename;

    bool flag_changed = false;

public:
    CodeEditor(QWidget *parent = nullptr);   

    bool has_changed() const {
        return this->flag_changed;
    }

    void unset_changed() {
        this->flag_changed = false;
    }

    int get_size() const {
        return this->toPlainText().size();
    }

    /**
     * @brief Set filename
     * @param _filename
     */
    inline void set_filename(const QString& _filename) {
        this->filename = _filename;
    }

    /**
     * @brief Get filename
     * @return
     */
    inline const auto& get_filename() const {
        return this->filename;
    }

    /**
     * @brief Set line numbers
     * @param event
     */
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    /**
     * @brief Draw a vertical line at 80 column mark
     * @param event
     */
    void verticalLinePaintEvent(QPaintEvent *event);

    int lineNumberAreaWidth();

    void search(const QString& word);

protected:
    void resizeEvent(QResizeEvent *event) override;

    void paintEvent(QPaintEvent *event) override {
        // base class
        QPlainTextEdit::paintEvent(event);

        // draw ruler
        this->verticalLinePaintEvent(event);
    }

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);

    void highlightCurrentLine();

    void updateLineNumberArea(const QRect &rect, int dy);

    void slot_changed();

    /**
     * @brief Move to top of file
     */
    void home();

    /**
     * @brief Move to to bottom of file
     */
    void end();

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget {

public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif
