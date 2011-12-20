#include "libavgraphicsitem.h"

LibAvGraphicsItem::LibAvGraphicsItem(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
}
QRectF LibAvGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(0,0), surfaceFormat().sizeHint());
}
void LibAvGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //TODO: steal/modify code from LibAvGraphicsItem1, since we are now using a new design (it might be a copy/paste though who knows)
}
QList<QVideoFrame::PixelFormat> LibAvGraphicsItem::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}
bool LibAvGraphicsItem::present(const QVideoFrame &frame)
{
    //TODO: steal/modify code from LibAvGraphicsItem1, since we are now using a new design (it might be a copy/paste though who knows)
    return true;
}
