#include "classeditordialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "designequalsimplementationclass.h"

//modeless yet still cancelable would be best, but for now i'll settle for modal and cancelable. actually fuck that shit, the editor is going to modify the backend object directly for now (fuck the police)
ClassEditorDialog::ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setWindowTitle(tr("Class Editor"));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *classNameRow = new QHBoxLayout();
    QLabel *classNameLabel = new QLabel(tr("Class &Name:"));
    QLineEdit *m_ClassNameLineEdit = new QLineEdit(classToEdit->ClassName);
    classNameLabel->setBuddy(m_ClassNameLineEdit);
    classNameRow->addWidget(classNameLabel);
    classNameRow->addWidget(m_ClassNameLineEdit);

    QHBoxLayout *validStateNameRow = new QHBoxLayout();
    QLabel *validStateNameLabel = new QLabel(tr("Optional state name: ")); //TODOreq: use for RAII constructor, a better name for the "initialized" signal, and the async initialize slot itself (RAII constructor calls said slot directly)
    QString validStateTooltip("Choose an arbitrary name for when this class/object is valid (open, ready, etc). \"is\" (ex: isReady) will be prepended to it:");
    validStateNameLabel->setToolTip(validStateTooltip);
    QLineEdit *m_ValidStateNameLineEdit = new QLineEdit();
    m_ValidStateNameLineEdit->setToolTip(validStateTooltip);
    validStateNameLabel->setBuddy(m_ValidStateNameLineEdit);
    validStateNameRow->addWidget(validStateNameLabel);
    validStateNameRow->addWidget(m_ValidStateNameLineEdit);

    QGroupBox *quickMemberAddGroupBox = new QGroupBox(tr("&Quick Add"));
    QHBoxLayout *quickMemberAddRow = new QHBoxLayout();
    QLineEdit *m_QuickMemberAddLineEdit = new QLineEdit();
    m_QuickMemberAddLineEdit->setPlaceholderText(tr("New member signature..."));
    QPushButton *quickAddNewPropertyButton = new QPushButton(tr("Property"));
    QPushButton *quickAddNewSignalButton = new QPushButton(tr("Signal"));
    QPushButton *quickAddNewSlotButton = new QPushButton(tr("Slot"));
    quickMemberAddRow->addWidget(m_QuickMemberAddLineEdit);
    quickMemberAddRow->addWidget(quickAddNewPropertyButton);
    quickMemberAddRow->addWidget(quickAddNewSignalButton);
    quickMemberAddRow->addWidget(quickAddNewSlotButton);
    quickMemberAddGroupBox->setLayout(quickMemberAddRow);

    myLayout->addLayout(classNameRow);
    myLayout->addLayout(validStateNameRow);
    myLayout->addWidget(quickMemberAddGroupBox);

    setLayout(myLayout);

    connect(quickAddNewPropertyButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewPropertyButtonClicked()));
    connect(quickAddNewSignalButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSignalButtonClicked()));
    connect(quickAddNewSlotButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSlotButtonClicked()));
}
void ClassEditorDialog::handleQuickAddNewPropertyButtonClicked()
{

}
void ClassEditorDialog::handleQuickAddNewSignalButtonClicked()
{

}
void ClassEditorDialog::handleQuickAddNewSlotButtonClicked()
{
    //TODOoptional: parse the method definition. i looked into using libclang, but was unable to get any of the examples working :-/. i could write a primitive basic parser, but also feel it might not be worth the effort since i'm definitely reinventing a wheel (and i know it would be limited in use and break easy)
}
