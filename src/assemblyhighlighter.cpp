#include "assemblyhighlighter.h"

AssemblyHighlighter::AssemblyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    this->keyword_format.setForeground(Qt::cyan);
    const QString keyword_patterns[] = {
        QStringLiteral("\\borg\\b"), QStringLiteral("\\badd\\b"), QStringLiteral("\\band\\b"),
        QStringLiteral("\\bor\\b"), QStringLiteral("\\bld\\b"), QStringLiteral("\\bDB\\b"),
        QStringLiteral("\\bjp\\b")
    };
    for (const QString &pattern : keyword_patterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = this->keyword_format;
        this->highlighting_rules.append(rule);
    }

    // comments
    this->single_line_comment_format.setForeground(Qt::gray);
    this->single_line_comment_format.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("^\\s*;.*$"));
    rule.format = single_line_comment_format;
    this->highlighting_rules.append(rule);

    // labels
    this->keyword_format.setForeground(Qt::white);
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
