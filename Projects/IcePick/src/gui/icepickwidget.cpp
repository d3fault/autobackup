#include "icepickwidget.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QDate>
#include <QCheckBox>

IcePickWidget::IcePickWidget(QWidget *parent)
    : QWidget(parent)
{
    m_YearRange.first = 0;
    m_YearRange.second = QDate::currentDate().year();

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *controlsRow = new QHBoxLayout();

    QSpinBox *minYearSpinBox = new QSpinBox();
    minYearSpinBox->setPrefix(tr("Min Year: "));
    minYearSpinBox->setRange(m_YearRange.first, m_YearRange.second);
    minYearSpinBox->setValue(m_YearRange.first);
    connect(minYearSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleMinYearChanged(int)));

    QSpinBox *maxYearSpinBox = new QSpinBox();
    maxYearSpinBox->setPrefix(tr("Max Year: "));
    maxYearSpinBox->setRange(m_YearRange.first, m_YearRange.second);
    maxYearSpinBox->setValue(m_YearRange.second);
    connect(maxYearSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleMaxYearChanged(int)));

    m_ForceRefreshCacheCheckbox = new QCheckBox(tr("Force Refresh Cache"));
    m_ForceRefreshCacheCheckbox->setChecked(false);

    QPushButton *pickIceButton = new QPushButton(tr("Pick random icefilms entry within range"));

    controlsRow->addWidget(minYearSpinBox);
    controlsRow->addWidget(maxYearSpinBox);
    controlsRow->addWidget(m_ForceRefreshCacheCheckbox);
    controlsRow->addWidget(pickIceButton);

    m_Output = new QTextEdit();

    myLayout->addLayout(controlsRow);
    myLayout->addWidget(m_Output);

    setLayout(myLayout);

    connect(pickIceButton, SIGNAL(clicked()), this, SLOT(handlePickIceButtonClicked()));
}
void IcePickWidget::handleMsg(const QString &msg)
{
    m_Output->append(msg);
}
void IcePickWidget::handleFinishedPickingIce(bool success, IceEntry iceEntry)
{
    m_Output->append(QString("pick ice ") + QString(success ? "success" : "failed"));
    if(success)
    {
        QString output;
        output.append(iceEntry.Title);
        output.append(" - ");
        output.append(QString::number(iceEntry.Year) + " - ");
        output.append(ICEFILMS_ENTRY_URL(iceEntry.Url));
        m_Output->append(output);

        /*
        QString htmlOutput;
        htmlOutput.append("<a href='");
        htmlOutput.append(ICEFILMS_ENTRY_URL(iceEntry.Url));
        htmlOutput.append("'>");
        htmlOutput.append(iceEntry.Title + "</a>");
        m_Output->insertHtml(htmlOutput);*/
    }
}
void IcePickWidget::handleMinYearChanged(int newMin)
{
    m_YearRange.first = newMin;
}
void IcePickWidget::handleMaxYearChanged(int newMax)
{
    m_YearRange.second = newMax;
}
void IcePickWidget::handlePickIceButtonClicked()
{
    emit pickIceRequested(m_YearRange, m_ForceRefreshCacheCheckbox->isChecked());
}
