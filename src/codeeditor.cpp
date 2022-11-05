#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>

/**
 * @brief Custom class for code editing
 * @param parent
 */
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    connect(this, &CodeEditor::textChanged, this, &CodeEditor::slot_changed);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // create shortcuts
    QShortcut *shortcuthome = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Home), this);
    QObject::connect(shortcuthome, SIGNAL(activated()), this, SLOT(home()));

    QShortcut *shortcutend = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_End), this);
    QObject::connect(shortcutend, SIGNAL(activated()), this, SLOT(end()));
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::slot_changed() {
    this->flag_changed = true;
    QWidget* parent = this->parentWidget()->parentWidget();
    qDebug() << tr("Getting parent_tab: ") + tr(parent->metaObject()->className());
    QTabWidget* parent_tab = static_cast<QTabWidget*>(parent);

    int tab_id = parent_tab->indexOf(this);
    qDebug() << tab_id;
    if(tab_id >= 0) {
        parent_tab->setTabText(tab_id, this->filename + "*");
    }
}

void CodeEditor::search(const QString& word) {
    bool found = this->find(word);
    if(!found) {
        QTextCursor cursor(this->document());
        cursor.movePosition(QTextCursor::Start);
        this->setTextCursor(cursor);
        found = this->find(word);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(0xe8e4cf).lighter(110);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

/**
 * @brief Set line numbers
 * @param event
 */
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(0xe8e4cf));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

/**
 * @brief Draw a vertical line at 80 column mark
 * @param event
 */
void CodeEditor::verticalLinePaintEvent(QPaintEvent *event) {

    static const int COLWIDTH = 644 / 2;
    QPen pen;
    pen.setColor(QColor(0x33, 0x33, 0x33, 0x30));
    pen.setStyle(Qt::DashLine);

    QPainter painter(this->viewport()); // note we use viewport as its the one that actually draws
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setPen(pen);
    //painter.drawLine(COLWIDTH, 0, COLWIDTH, this->viewport()->height());
    painter.drawLine(COLWIDTH * 2, 0, COLWIDTH * 2, this->viewport()->height());
    this->update();
}

/**
 * @brief Move to top of file
 */
void CodeEditor::home() {
    QTextCursor cursor(this->document());
    cursor.movePosition(QTextCursor::Start);
    this->setTextCursor(cursor);
}

/**
 * @brief Move to to bottom of file
 */
void CodeEditor::end() {
    QTextCursor cursor(this->document());
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
}
