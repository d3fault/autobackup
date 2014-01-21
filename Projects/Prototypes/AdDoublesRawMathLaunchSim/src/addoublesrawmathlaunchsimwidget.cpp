#include "addoublesrawmathlaunchsimwidget.h"

#include <QHBoxLayout>

AdDoublesRawMathLaunchSimWidget::AdDoublesRawMathLaunchSimWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QLabel *minPriceLabel = new QLabel("Minimum Price:");
    m_MinPriceLineEdit = new QLineEdit("0.001");
    QHBoxLayout *minPriceRow = new QHBoxLayout();
    minPriceRow->addWidget(minPriceLabel);
    minPriceRow->addWidget(m_MinPriceLineEdit);

    QLabel *slotFillerLengthLabel = new QLabel("Slot Filler Length  In Hours:");
    m_SlotFillerLengthHoursLineEdit = new QLineEdit("24");
    QHBoxLayout *slotFillerLengthRow = new QHBoxLayout();
    slotFillerLengthRow->addWidget(slotFillerLengthLabel);
    slotFillerLengthRow->addWidget(m_SlotFillerLengthHoursLineEdit);

    QPushButton *launchButton = new QPushButton("Launch");
    QPushButton *buyButton = new QPushButton("Buy At Current Price");
    QPushButton *stopButton = new QPushButton("Stop");

    m_Debug = new QPlainTextEdit();

    myLayout->addLayout(minPriceRow);
    myLayout->addLayout(slotFillerLengthRow);
    myLayout->addWidget(launchButton);
    myLayout->addWidget(buyButton);
    myLayout->addWidget(stopButton);
    myLayout->addWidget(m_Debug);
    setLayout(myLayout);

    connect(launchButton, SIGNAL(clicked()), this, SLOT(handleLaunchClicked()));
    connect(buyButton, SIGNAL(clicked()), this, SLOT(handleBuyButtonClicked()));
    connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stopRequested()));
}
AdDoublesRawMathLaunchSimWidget::~AdDoublesRawMathLaunchSimWidget()
{
}
void AdDoublesRawMathLaunchSimWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void AdDoublesRawMathLaunchSimWidget::handleSlotAvailableAtPrice(uint slotFillerIndex, double priceAvailableToPurchaseAt)
{
    m_SlotFillerIndexAvailableForSale = slotFillerIndex;
    m_PriceAvailableToPurchaseAt = priceAvailableToPurchaseAt;
    handleD("Slot #" + QString::number(slotFillerIndex) + " is available to purchase at: " + QString::number(priceAvailableToPurchaseAt, 'f', 8) + " btc");
}
void AdDoublesRawMathLaunchSimWidget::handleSlotFillerForSaleChanged(uint newSlotFillerForSaleIndex)
{
    handleD("Someone purchased a slot, so the new index for sale is: " + QString::number(newSlotFillerForSaleIndex));
}
void AdDoublesRawMathLaunchSimWidget::handleSlotFillerOnDisplayChanged(int newSlotFillerOnDisplayIndex)
{
    handleD("A slot on display expired, here is the new index on display: " + QString::number(newSlotFillerOnDisplayIndex));
}
void AdDoublesRawMathLaunchSimWidget::handleLaunchClicked()
{
    emit launchRequested(m_MinPriceLineEdit->text().toDouble(), m_SlotFillerLengthHoursLineEdit->text().toUInt());
}
void AdDoublesRawMathLaunchSimWidget::handleBuyButtonClicked()
{
    emit buyAttemptRequested(m_SlotFillerIndexAvailableForSale, m_PriceAvailableToPurchaseAt);
}
