#ifndef FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORWIDGET_H
#define FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORWIDGET_H

#include <QWidget>

class QLineEdit;
class QPlainTextEdit;

class FunctionDecomposerWithNewTypeDetectorWidget : public QWidget
{
    Q_OBJECT
public:
    FunctionDecomposerWithNewTypeDetectorWidget(QWidget *parent = 0);
private:
    QLineEdit *m_FunctionDeclarationLineEdit;
    QPlainTextEdit *m_Output;
signals:
    void parseFunctionDeclarationRequested(const QString &functionDeclaration);
public slots:
    void handleSyntaxErrorDetected();
    void handleO(const QString &msg);
private slots:
    void handleParseFunctionDeclarationButtonClicked();
};

#endif // FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORWIDGET_H
