#include "mouseormotion2linuxfb.h"

#include <QCoreApplication>
#include <QDebug>

#include <fcntl.h>
#include <sys/ioctl.h>

//I'm not sure if I need a backend thread for this app. Even though it's a GUI app (QT += gui), there's no actual GUI to block! One reason it might warrant a backend thread though, is so the system can communicate with it still. It might think it's "not responding" etc if the system overall is under high load and motion detection is lagging. Whatever, can always throw that bitch on a backend thread later...
//TODOreq: a way to close/stop the application (I want my destructors to run). Perhaps a system tray icon? I need to do some research in order to get 'shutdown' of OS to cleanly stop my apps. aboutToQuit() comes to mind, but I had some reason for not using it long ago. These days I make my apps quit themselves
MouseOrMotion2LinuxFb::MouseOrMotion2LinuxFb(QObject *parent)
    : QObject(parent)
{
    QStringList argz = qApp->arguments();
    argz.removeFirst(); //app name

    QString framebufferFilePath = "/dev/fb1";
    if(!argz.isEmpty())
        framebufferFilePath = argz.takeFirst();

    if(!setupFramebuffer(framebufferFilePath))
    {
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

    MouseOrMotionRectGrabberWithPeriodOfInactivityDetection *rectGrabber = new MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(this);
    connect(rectGrabber, SIGNAL(mouseOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)), this, SLOT(handleMouseOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)));
    rectGrabber->startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(QSize(m_FramebufferVariableInformation.xres,m_FramebufferVariableInformation.yres/*wtf is a virtual resolution?*/), 1000, 10000, 1.0, QVariant::Image);
}
bool MouseOrMotion2LinuxFb::setupFramebuffer(const QString &framebufferFilePath)
{
    std::string framebufferFilePathStdString = framebufferFilePath.toStdString();
    int framebufferFile = open(framebufferFilePathStdString.c_str(), O_RDWR);
    if(framebufferFile == -1)
    {
        qDebug() << "failed to open framebuffer:" << framebufferFilePath;
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

    switch(m_FramebufferVariableInformation.bits_per_pixel)
    {
    case 32:
        m_FramebufferImageFormat =  QImage::Format_RGB32; //I read on the internets that the framebuffer expects BGR[A] instead, but this should work at least... although the result might look funkay color-wise. I can't find a BGRA QIMage format so wtf how I doit maaan. flip the eindianness? woot just found rgbSwapped thx cutie
        break;
    case 16:
        m_FramebufferImageFormat = QImage::Format_RGB16;
        break;
    default:
        qDebug() << "framebuffer bpp not 32 or 16 wtfz? bpp = " << m_FramebufferVariableInformation.bits_per_pixel;
        return false;
        break;
    }
    return true;
}
void MouseOrMotion2LinuxFb::handleMouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion)
{
    Q_UNUSED(positionOnDesktopWhereMouseOrMotionOccured)
    Q_UNUSED(mouseOrMotion)

    QImage mouseOrMotionImage = rectAroundMouseOrMotion.value<QImage>();
    if(mouseOrMotionImage.format() != m_FramebufferImageFormat)
        mouseOrMotionImage = mouseOrMotionImage.convertToFormat(m_FramebufferImageFormat);
    mouseOrMotionImage = mouseOrMotionImage.rgbSwapped(); //too bad this means two conversions xD, why Qt doesn't have FORMAT_BGR* is beyond me
    Q_ASSERT(m_FramebufferFixedInformation.smem_len == mouseOrMotionImage.byteCount());
    memcpy(m_MemoryMappedFramebuffer.data(), mouseOrMotionImage.constBits(), m_FramebufferFixedInformation.smem_len);
}
