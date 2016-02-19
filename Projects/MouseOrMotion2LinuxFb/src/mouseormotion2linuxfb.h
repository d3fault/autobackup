#ifndef MOUSEORMOTION2LINUXFB_H
#define MOUSEORMOTION2LINUXFB_H

#include <QObject>

#include <QScopedPointer>
#include <QImage>

#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

struct FramebufferFileCloserAndDeleter
{
    static inline void cleanup(int *framebufferFile)
    {
        close(*framebufferFile);
        delete framebufferFile;
    }
};
class MemoryMappedFramebufferUnmapper
{
public:
    explicit MemoryMappedFramebufferUnmapper(char *memoryMappedFrameBuffer = 0, quint32 smemLen = 0)
        : m_MemoryMappedFrameBuffer(memoryMappedFrameBuffer)
        , m_SmemLen(smemLen)
    { }
    char *data() { return m_MemoryMappedFrameBuffer; }
    void reset(char *memoryMappedFrameBuffer = 0, quint32 smemLen = 0)
    {
        if(m_MemoryMappedFrameBuffer)
            munmap(m_MemoryMappedFrameBuffer, m_SmemLen);
        m_MemoryMappedFrameBuffer = memoryMappedFrameBuffer;
        m_SmemLen = smemLen;
    }
    ~MemoryMappedFramebufferUnmapper()
    {
        reset();
    }
private:
    Q_DISABLE_COPY(MemoryMappedFramebufferUnmapper)
    char *m_MemoryMappedFrameBuffer;
    quint32 m_SmemLen;
};
class MouseOrMotion2LinuxFb : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotion2LinuxFb(QObject *parent = 0);
private:
    QScopedPointer<int, FramebufferFileCloserAndDeleter> m_FramebufferFile;
    MemoryMappedFramebufferUnmapper m_MemoryMappedFramebuffer;
    fb_var_screeninfo m_FramebufferVariableInformation;
    fb_fix_screeninfo m_FramebufferFixedInformation;
    QImage::Format m_FramebufferImageFormat;

    bool setupFramebuffer(const QString &framebufferFilePath);
private slots:
    void handleMouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
};

#endif // MOUSEORMOTION2LINUXFB_H
