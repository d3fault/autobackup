#ifndef LINEEDITWITHPUBLICCURSORRECT_H
#define LINEEDITWITHPUBLICCURSORRECT_H

#include <QLineEdit>

class LineEditWithPublicCursorRect : public QLineEdit
{
public:
    explicit LineEditWithPublicCursorRect(QWidget* parent=0)
        : QLineEdit(parent)
    { }

    QRect publicCursorRect() const
    {
        return cursorRect();
    }
};

#endif // LINEEDITWITHPUBLICCURSORRECT_H
