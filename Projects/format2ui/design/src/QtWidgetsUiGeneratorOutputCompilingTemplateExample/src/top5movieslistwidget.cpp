#include "top5movieslistwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

Top5MoviesListWidget::Top5MoviesListWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayoutAndAddNewRowButtonBelow = new QVBoxLayout(this);

    m_MyLayout = new QVBoxLayout();
    addNewRow();

    myLayoutAndAddNewRowButtonBelow->addLayout(m_MyLayout);
    QPushButton *addNewRowButton = new QPushButton(tr("Add New Row"));
    myLayoutAndAddNewRowButtonBelow->addWidget(addNewRowButton);

    connect(addNewRowButton, &QPushButton::clicked, this, &Top5MoviesListWidget::handledAddNewRowButtonClicked);
}
//QVariantList Top5MoviesListWidget::variantList() const
//^actually WidgetList will be QVariantList based, but top5Movies is QStringList based (KISS)
QStringList Top5MoviesListWidget::top5Movies() const
{
    //return m_Top5Movies;
    QStringList ret;
    for(QList<QLineEdit*>::const_iterator it = m_Top5MoviesLineEdits.constBegin(); it != m_Top5MoviesLineEdits.constEnd(); ++it)
    {
        QLineEdit *lineEdit = *it;
        ret << lineEdit->text();
    }
    return ret;
}
void Top5MoviesListWidget::addNewRow()
{
    QHBoxLayout *labelAndLineEditRow = new QHBoxLayout();
    int currentNum = m_Top5MoviesLineEdits.size();
    QString humanReadableWithNum("Top 5 Movies #" + QString::number(currentNum));
    QLabel *label = new QLabel(humanReadableWithNum);
    labelAndLineEditRow->addWidget(label);
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText(humanReadableWithNum);
    labelAndLineEditRow->addWidget(lineEdit);
    m_Top5MoviesLineEdits.append(lineEdit);
    m_MyLayout->addLayout(labelAndLineEditRow);
}
void Top5MoviesListWidget::handledAddNewRowButtonClicked()
{
    addNewRow();
}
