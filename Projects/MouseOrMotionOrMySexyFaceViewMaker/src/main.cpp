#include <QApplication>

#include "mouseormotionormysexyfaceviewmakergui.h"

//TODOoptimization: i think this app is bottlenecking at scaling the "my face" images into thumbnails, and i think it can be solved by using another ffmpeg instance to do the scaling instead of qt's QImage/QPixmap .scale(). So it could go like this: v4l2 -> ffmpeg(crop-bars) -> tee(thisAppFullSizeFaceBuffer,nextFfmpegInstance) -> nextFfmpegInstance(scaleToThumbnailSize) -> thisAppThumbnailFaceBuffer. I just think that ffmpeg will probably be tons faster at scaling the images, but could be wrong
//Completely aside and OT: Qt should wrap ffmpeg (it would make QCamera not be shit, for starters), but because of it's stupid CLA, it can't. It COULD because they are both LGPL, but it can't because Qt requires CLA to accept contributions. CLA is getting in the way of the quality software. In conclusion, CLAs and copyright assignment defeat purpose of FLOSS

//would be trivial to save the image instead of (or in addition to) presenting it, but since i don't need/want that functionality, i'm not going to code it...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotionOrMySexyFaceViewMakerGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
