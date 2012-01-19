#include "videographicsitem.h"

videoGraphicsItem::videoGraphicsItem(QGraphicsItem *parent) :
    QGraphicsItem(parent), m_ImageFormat(QImage::Format_Invalid), m_ImageWidth(0), m_ImageHeight(0), m_FramePainted(false)
{
}
QRectF videoGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(0,0), surfaceFormat().sizeHint());
}
void videoGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_CurrentFrame.map(QAbstractVideoBuffer::ReadOnly))
    {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -boundingRect().height());
        }

        //QImage *test = new QImage();
        //test->QImage();
        //^^if we can do this, we MIGHT be able to use the same image pointer on the GUI thread inside of paint(). re-call the constructor so it encapsulates a different buffer... but NOT re-allocate the QImage pointer. then again, doesn't the constructor do just that? fml... or is it the "new" keyword. no clue...

        painter->drawImage(boundingRect(), QImage(
                m_CurrentFrame.bits(),
                m_ImageWidth,
                m_ImageHeight,
                m_ImageFormat));

        painter->setTransform(oldTransform);

        m_FramePainted = true;

        m_CurrentFrame.unmap();
    }
}
QList<QVideoFrame::PixelFormat> videoGraphicsItem::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
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
bool videoGraphicsItem::start(const QVideoSurfaceFormat &format)
{
    bool ret = false;
    if(isFormatSupported(format))
    {
        m_FramePainted = true;
        ret = QAbstractVideoSurface::start(format);
        prepareGeometryChange();
    }
    return ret;
}
void videoGraphicsItem::stop()
{
    m_CurrentFrame = QVideoFrame();
    m_FramePainted = false;
    QAbstractVideoSurface::stop();
}
bool videoGraphicsItem::present(const QVideoFrame &frame)
{
    if(!m_FramePainted)
    {
        if(!QAbstractVideoSurface::isActive())
        {
            //setError(StoppedError);
            //todo: handle the error case where our previous frame has not yet been painted and our abstract video surface is not active (i guess it should be?)
        }
        return false;
    }
    else
    {
        m_CurrentFrame = frame;
        m_FramePainted = false;

        update();

        return true;
    }
}
void videoGraphicsItem::setupVideoItem(int width, int height, QImage::Format imageFormat)
{
    m_ImageFormat = imageFormat;
    m_ImageWidth = width;
    m_ImageHeight = height;

    //create dummy image to make video frame from
    QImage image(width, height, imageFormat);

    //wrap image in video frame
    QVideoFrame frame(image); //QVideoFrame isn't even needed (except to get surface format info from etc. see below).. since when we paint we're really using a QImage... and it's underlying image data is already mapped.. since we own the pointer to the malloc'd memory. coming back, actually i'm wrong since we inherit QAbstractVideoSurface. present() can't take anything except a QVideoFrame. why the fuck do i need to be a QAbstractVideoSurface anyways? what benefits does this bring me? probably some potential future proofing... but that's about it. waste of fucking time so far if you ask me... especially considering paint() uses a god damn image.

    if(!frame.isValid())
    {
        //TODO:
        return;
        //emit debug or something
        //maybe set a m_VideoItemSetUp = true; at the end of this function. false by default. then check it whenever a frame tries to be presented
    }

    //get the default pixel format
    QVideoSurfaceFormat defaultFormat = this->surfaceFormat();

    //see if our frame's pixel format matches the default one
    if (frame.pixelFormat() != defaultFormat.pixelFormat() || frame.size() != defaultFormat.frameSize())
    {
        //if not, start video item with our newly created format
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());
        this->start(format);
    }
    else //our frame's format matches the surface's format.. so just start it
    {
        this->start(defaultFormat);
    }
}
