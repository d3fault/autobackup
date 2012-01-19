#ifndef LIBAVGRAPHICSITEM3TEST_H
#define LIBAVGRAPHICSITEM3TEST_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>

#include "videoplayerwidget.h"

class libAvGraphicsItem3test : public QWidget
{
    Q_OBJECT
public:
    libAvGraphicsItem3test(QWidget *parent = 0);
    ~libAvGraphicsItem3test();
private:
    QVBoxLayout *m_Layout;
    videoPlayerWidget *m_VideoPlayerWidget;
    QPlainTextEdit *m_DebugOutput;
};

#endif // LIBAVGRAPHICSITEM3TEST_H
