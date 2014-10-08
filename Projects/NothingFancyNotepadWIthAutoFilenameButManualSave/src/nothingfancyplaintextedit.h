#ifndef NOTHINGFANCYPLAINTEXTEDIT_H
#define NOTHINGFANCYPLAINTEXTEDIT_H

#include <QPlainTextEdit>

class NothingFancyPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit NothingFancyPlainTextEdit(QWidget *parent = 0);

    bool textHasBeenEditted() const;
    void setTextHasBeenEditted(bool textHasBeenEditted);
private:
    bool m_TextHasBeenEditted;
private slots:
    void setTextHasBeenEdittedToTrue();
};

#endif // NOTHINGFANCYPLAINTEXTEDIT_H
