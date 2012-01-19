#ifndef VIDEOGRAPHICSITEM_H
#define VIDEOGRAPHICSITEM_H

#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoSurfaceFormat>
#include <QtGui/QGraphicsItem>
#include <QPainter>

class videoGraphicsItem :public QAbstractVideoSurface, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    videoGraphicsItem(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
private:
    QVideoFrame m_CurrentFrame;

    QImage::Format  m_ImageFormat;
    int             m_ImageWidth;
    int             m_ImageHeight;

    bool m_FramePainted;
public slots:
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    bool present(const QVideoFrame &frame);

    void setupVideoItem(int width, int height, QImage::Format imageFormat);
};

#endif // VIDEOGRAPHICSITEM_H
