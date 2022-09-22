#ifndef ASSEMBLYHIGHLIGHTER_H
#define ASSEMBLYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QObject>

class AssemblyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    AssemblyHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlighting_rules;

    QRegularExpression single_line_comment_expression;
    QTextCharFormat keyword_format;
    QTextCharFormat single_line_comment_format;
};

#endif // ASSEMBLYHIGHLIGHTER_H
