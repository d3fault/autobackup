#ifndef TODOLISTWIDGET_H
#define TODOLISTWIDGET_H

#include <QWidget>

class QLineEdit;

class TodoListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TodoListWidget(QWidget *parent = 0);
private:
    QLineEdit *m_NewTodoItemLineEdit;
private slots:
    void addNewTodoItem();
};

#endif // TODOLISTWIDGET_H
