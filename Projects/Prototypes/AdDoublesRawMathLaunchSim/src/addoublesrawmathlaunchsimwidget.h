#ifndef ADDOUBLESRAWMATHLAUNCHSIMWIDGET_H
#define ADDOUBLESRAWMATHLAUNCHSIMWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

class AdDoublesRawMathLaunchSimWidget : public QWidget
{
    Q_OBJECT
public:
    AdDoublesRawMathLaunchSimWidget(QWidget *parent = 0);
    ~AdDoublesRawMathLaunchSimWidget();
private:
    QLineEdit *m_MinPriceLineEdit;
    QLineEdit *m_SlotFillerLengthHoursLineEdit;
    QPlainTextEdit *m_Debug;
    void handleD(const QString &msg);

    uint m_SlotFillerIndexAvailableForSale;
    double m_PriceAvailableToPurchaseAt;
signals:
    void launchRequested(double minPrice, unsigned int slotFillerLengthHours);
    void buyAttemptRequested(unsigned int slotFillerNum, double priceToTryToBuyAt);
    void stopRequested();
public slots:
    void handleSlotAvailableAtPrice(uint slotFillerIndex ,double priceAvailableToPurchaseAt);
    void handleSlotFillerForSaleChanged(uint newSlotFillerForSaleIndex);
    void handleSlotFillerOnDisplayChanged(int newSlotFillerOnDisplayIndex);
private slots:
    void handleLaunchClicked();
    void handleBuyButtonClicked();
};

#endif // ADDOUBLESRAWMATHLAUNCHSIMWIDGET_H
