#include "couchbasenotepadwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>

CouchbaseNotepadWidget::CouchbaseNotepadWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *keyRow = new QHBoxLayout();
    QLabel *keyLabel = new QLabel(tr("Key:"));
    m_KeyLineEdit = new QLineEdit();
    keyRow->addWidget(keyLabel);
    keyRow->addWidget(m_KeyLineEdit, 1);

    m_Value = new QPlainTextEdit();

    QHBoxLayout *getStoreRow = new QHBoxLayout();
    QPushButton *getButton = new QPushButton(tr("Get"));
    QPushButton *addButton = new QPushButton(tr("Add"));
    getStoreRow->addWidget(getButton);
    getStoreRow->addWidget(addButton); //TODOoptional: 'overwrite if existing' checkbox

    myLayout->addLayout(keyRow);
    myLayout->addWidget(new QLabel(tr("Value:")));
    myLayout->addWidget(m_Value);
    myLayout->addLayout(getStoreRow);

    setLayout(myLayout);


    connect(getButton, SIGNAL(clicked()), this, SLOT(handleGetButtonClicked()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(handleAddButtonClicked()));
}
void CouchbaseNotepadWidget::handleGetCouchbaseNotepadDocFinished(const QString &key, const QString &value)
{
    Q_UNUSED(key)
    m_Value->clear();
    m_Value->appendPlainText(value);
}
void CouchbaseNotepadWidget::handleAddCouchbaseNotepadDocFinished(const QString &key, const QString &value)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    QMessageBox::information(this, tr("Success"), tr("Add was successful"));
}
void CouchbaseNotepadWidget::handleGetButtonClicked()
{
    //TODOoptional: sanitize. filter or convert illegal characters, such as spaces
    if(m_KeyLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Key can not be empty"));
        return;
    }
    emit getCouchbaseNotepadDocByKeyRequested(m_KeyLineEdit->text());
}
void CouchbaseNotepadWidget::handleAddButtonClicked()
{
    if(m_KeyLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Key can not be empty"));
        return;
    }
    //value CAN be empty
    emit addCouchbaseNotepadDocByKeyRequested(m_KeyLineEdit->text(), m_Value->document()->toPlainText());
}
