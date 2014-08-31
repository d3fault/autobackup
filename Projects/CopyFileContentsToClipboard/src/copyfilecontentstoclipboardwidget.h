#ifndef COPYFILECONTENTSTOCLIPBOARDWIDGET_H
#define COPYFILECONTENTSTOCLIPBOARDWIDGET_H

#include <QWidget>

class QCheckBox;
class QPlainTextEdit;

class CopyFileContentsToClipboardWidget : public QWidget
{
    Q_OBJECT
public:
    CopyFileContentsToClipboardWidget(QWidget *parent = 0);
private:
    QCheckBox *m_TreatAsBinaryCheckbox;
    QPlainTextEdit *m_Output;
private slots:
    void copyFileContentsToClipboardWidget(const QString &filePath);
};

#endif // COPYFILECONTENTSTOCLIPBOARDWIDGET_H
