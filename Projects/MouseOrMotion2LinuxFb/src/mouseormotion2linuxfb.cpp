#include "mouseormotion2linuxfb.h"

#include <QCoreApplication>
#include <QDebug>
#include <QImage>

#include <fcntl.h>
#include <sys/ioctl.h>

//I'm not sure if I need a backend thread for this app. Even though it's a GUI app (QT += gui), there's no actual GUI to block! One reason it might warrant a backend thread though, is so the system can communicate with it still. It might think it's "not responding" etc if the system overall is under high load and motion detection is lagging. Whatever, can always throw that bitch on a backend thread later...
MouseOrMotion2LinuxFb::MouseOrMotion2LinuxFb(QObject *parent)
    : QObject(parent)
{
    if(!setupFb())
    {
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

    MouseOrMotionRectGrabberWithPeriodOfInactivityDetection *rectGrabber = new MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(this);
    connect(rectGrabber, SIGNAL(mouseOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)), this, SLOT(handleMouseOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)));
    rectGrabber->startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(QSize(160,128), 1000, 10000, 1.0, QVariant::Image);
}
bool MouseOrMotion2LinuxFb::setupFb()
{
    int framebufferFile = open("/dev/fb1", O_RDWR); //TODOreq: allow specifying fbN (path)
    if(framebufferFile == -1)
    {
        qDebug() << "failed to open framebuffer";
        return false;
    }
    m_FramebufferFile.reset(new int(framebufferFile));
    if(ioctl(framebufferFile, FBIOGET_FSCREENINFO, &m_FramebufferFixedInformation))
    {
        //We require information...
        qDebug() << "failed to get framebuffer fixed information";
        return false;
    }
    if(ioctl(framebufferFile, FBIOGET_VSCREENINFO, &m_FramebufferVariableInformation))
    {
        //...in formation!!!
        qDebug() << "failed to get framebuffer variable information";
        return false;
    }
    //s_Smem_Len = m_FramebufferFixedInformation.smem_len; //hack. TODOreq: figure out (ask) the correct way to access member data during scoped pointer destruction O_o
    char *memoryMappedFramebuffer = (char*)mmap(0, m_FramebufferFixedInformation.smem_len, (PROT_READ | PROT_WRITE), MAP_SHARED, framebufferFile, 0);
    if(memoryMappedFramebuffer == MAP_FAILED)
    {
        qDebug() << "failed to create memory mapped framebuffer";
        return false;
    }
    m_MemoryMappedFramebuffer.reset(memoryMappedFramebuffer, m_FramebufferFixedInformation.smem_len);
    return true;
}
void MouseOrMotion2LinuxFb::handleMouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion)
{
    Q_UNUSED(positionOnDesktopWhereMouseOrMotionOccured)
    Q_UNUSED(mouseOrMotion)

    QImage mouseOrMotionImage = rectAroundMouseOrMotion.value<QImage>();
    memcpy(m_MemoryMappedFramebuffer.data(), static_cast<const uchar*>(mouseOrMotionImage.bits()), qMin(m_FramebufferFixedInformation.smem_len, mouseOrMotionImage.byteCount())); //TODOreq: this probably won't work because of QImage::Format_ shiz and/or bpp etc etc, but this is the rough idea
}
