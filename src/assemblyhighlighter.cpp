#include "assemblyhighlighter.h"

AssemblyHighlighter::AssemblyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // opcodes
    this->keyword_format.setForeground(QColor(BASE08));
    const QStringList opcodes = {
        "NOP", "LD", "INC", "DEC", "RLCA", "EX", "ADD", "RRCA", "DJNZ", "RLA", "JR",
        "RRA", "DAA", "CPL", "SCF", "CCF", "HALT", "ADC", "SUB", "SBC", "AND", "XOR",
        "OR", "CP", "RET", "POP", "JP", "CALL", "PUSH", "RST", "OUT", "EXX", "IN",
        "DI", "EI", "LDIR"
    };
    for(int i=0; i<opcodes.size(); i++) {
        rule.pattern = QRegularExpression(tr("\\b") + opcodes[i].toLower() + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = this->keyword_format;
        this->highlighting_rules.append(rule);
    }

    // special keywords
    this->keyword_format.setForeground(QColor(BASE0F));
    const QStringList keywords = {
        "DB", "DW", "EQU", "ORG", "FNAME", "FORG", "INCLUDE", "INCBIN",
        "PHASE", "DEPHASE", "RB", "RW"
    };
    for(int i=0; i<keywords.size(); i++) {
        rule.pattern = QRegularExpression(tr("\\b") + keywords[i].toLower() + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = this->keyword_format;
        this->highlighting_rules.append(rule);
    }

    // labels
    this->keyword_format.setForeground(QColor(BASE0C));
    rule.pattern = QRegularExpression(QStringLiteral("^[A-Za-z0-9_]+:\\s*$"));
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);

    // sublabels
    this->keyword_format.setForeground(QColor(BASE0C));
    rule.pattern = QRegularExpression(QStringLiteral("^\\.[A-Za-z0-9_]+:\\s*"));
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);

    // numbers - regular
    this->keyword_format.setForeground(QColor(COL_NUMBERS));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+\\b"));
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);

    // numbers - hexadecimal (prepended by dollar and appended by h)
    this->keyword_format.setForeground(QColor(COL_NUMBERS));
    rule.pattern = QRegularExpression(QStringLiteral("\\$[0-9A-F]+\\b"), QRegularExpression::CaseInsensitiveOption);
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9A-F]+h\\b"), QRegularExpression::CaseInsensitiveOption);
    rule.format = this->keyword_format;
    this->highlighting_rules.append(rule);

    // comments --> needs to be the last rule as it needs to overrule other rules
    this->single_line_comment_format.setForeground(QColor(COL_COMMENTS));
    this->single_line_comment_format.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral(";.*$"));
    rule.format = single_line_comment_format;
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
