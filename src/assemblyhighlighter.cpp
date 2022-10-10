#include "assemblyhighlighter.h"

AssemblyHighlighter::AssemblyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // comments
    this->single_line_comment_format.setForeground(QColor(BASE03));
    this->single_line_comment_format.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral(";.*$"));
    rule.format = single_line_comment_format;
    this->highlighting_rules.append(rule);

    this->keyword_format.setForeground(QColor(BASE08));
    const QStringList opcodes = {
        "NOP", "LD", "INC", "DEC", "RLCA", "EX", "ADD", "RRCA", "DJNZ", "RLA", "JR",
        "RRA", "DAA", "CPL", "SCF", "CCF", "HALT", "ADC", "SUB", "SBC", "AND", "XOR",
        "OR", "CP", "RET", "POP", "JP", "CALL", "PUSH", "RST", "OUT", "EXX", "IN",
        "DI", "EI"
    };
    for(int i=0; i<opcodes.size(); i++) {
        rule.pattern = QRegularExpression(tr("\\b") + opcodes[i].toLower() + "\\b");
        rule.format = this->keyword_format;
        this->highlighting_rules.append(rule);
    }

    // labels
    this->keyword_format.setForeground(QColor(BASE0C));
    rule.pattern = QRegularExpression(QStringLiteral("^[A-Za-z0-9]+:\\s*$"));
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);
}

void AssemblyHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(this->highlighting_rules)) {
        QRegularExpressionMatchIterator match_iterator = rule.pattern.globalMatch(text);
        while (match_iterator.hasNext()) {
            QRegularExpressionMatch match = match_iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
