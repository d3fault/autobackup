#ifndef LIBAVGRAPHICSITEM_H
#define LIBAVGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoSurfaceFormat>

class LibAvGraphicsItem : public QAbstractVideoSurface, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit LibAvGraphicsItem(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
signals:

public slots:
    bool present(const QVideoFrame &frame);
};

#endif // LIBAVGRAPHICSITEM_H
