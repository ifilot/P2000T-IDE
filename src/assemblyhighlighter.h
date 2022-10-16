#ifndef ASSEMBLYHIGHLIGHTER_H
#define ASSEMBLYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QObject>

// define colors here

class AssemblyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    // color scheme: https://atelierbram.github.io/syntax-highlighting/atelier-schemes/dune/
    const unsigned int BASE00 = 0x20201d;
    const unsigned int BASE01 = 0x292824;
    const unsigned int BASE02 = 0x6e6b5e;
    const unsigned int BASE03 = 0x7d7a68;
    const unsigned int BASE04 = 0x999580;
    const unsigned int BASE05 = 0xa6a28c;
    const unsigned int BASE06 = 0xe8e4cf;
    const unsigned int BASE07 = 0xfefbec;
    const unsigned int BASE08 = 0xd73737;

    const unsigned int BASE0B = 0x60ac39;
    const unsigned int BASE0C = 0x1fad83;
    const unsigned int BASE0D = 0x6684e1;
    const unsigned int BASE0F = 0xd43552;

    const unsigned int COL_COMMENTS = BASE03;
    const unsigned int COL_NUMBERS = BASE0D;

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
