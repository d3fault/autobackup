#include <QApplication>

#include "mouseormotionormysexyfaceviewmakergui.h"

//TODOoptimization: i think this app is bottlenecking at scaling the "my face" images into thumbnails, and i think it can be solved by using another ffmpeg instance to do the scaling instead of qt's QImage/QPixmap .scale(). So it could go like this: v4l2 -> ffmpeg(crop-bars) -> tee(thisAppFullSizeFaceBuffer,nextFfmpegInstance) -> nextFfmpegInstance(scaleToThumbnailSize) -> thisAppThumbnailFaceBuffer. I just think that ffmpeg will probably be tons faster at scaling the images, but could be wrong
//Completely aside and OT: Qt should wrap ffmpeg (it would make QCamera not be shit, for starters), but because of it's stupid CLA, it can't. It COULD because they are both LGPL, but it can't because Qt requires CLA to accept contributions. CLA is getting in the way of the quality software. In conclusion, CLAs and copyright assignment defeat purpose of FLOSS
//TO DOneoptimization: will likely require some refactoring, but i don't need to check for motion for every frame presented. when running at low fps it isn't a big deal, but with higher fps it is the most likely reason i'm seeing slowdown. using a "percent of fps" to check for motion at first sounds like the best solution, but then for low fps situations it might end up sucking. so perhaps a % of fps (which is low, perhaps 10% or so) with a hard-"minimum" fps to check for motion. like idk 3-10fps. that way if I'm capping at 30fps I can still detect at 3fps, but if capping at 3fps it does NOT detect at .3 fps, since that would be too low. maybe the hard minimum should be 3 (which would mean i need to make 3fps the cap min too).
//^when current frame is deemed optimial for determining if change, compare against last frame deemed optimal, not "previous frame grabbed/presented". i was tempted to compare against previous frame grabbed/presented, but that would cause lots of motion (the typing of a single character, for example) to be missed
//TODOoptional: blinking cursors should be ignored in motion detection, but i'm still theorizing a formula that is the perfect balance of flexible (lots of blinking cursors in lots of apps) and rigid (the characters l/I/|/etc should still be detected)
//TODOoptional: iconify since will be open assloads

//would be trivial to save the image instead of (or in addition to) presenting it, but since i don't need/want that functionality, i'm not going to code it...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotionOrMySexyFaceViewMakerGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
