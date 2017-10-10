#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

using namespace stk;

#include <QtMultimedia/QAudioOutput>
#include <QSlider>
#include <QHBoxLayout>
#include <QMessageBox>

#include <QDebug>

#include "stkfileloopiodevice.h"

//TODOreq: debug what thread is calling this::stkTick .. because if stk isn't using it's own backend thread then I really don't have ANY clue how it's even functioning O_o. once I know it's using it's own thread, I should maybe use QAtomicInt to safely comm between the GUI and Stk -- the int I'm talking about of course is the one passed to pitchBend/whatever
//TODOmb: a slider for volume and other shiz stk provides
LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent)
    : QWidget(parent)
    , m_NumBufferFrames(RT_BUFFER_SIZE)
{
    setupGui();

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate((StkFloat)44100);

    input.reset(new stk::FileLoop);
    // Try to load the soundfile.
    try
    {
        input->openFile("/run/shm/audio.wav");
    }
    catch(StkError &stkError)
    {
        showStdStringError(stkError.getMessage());
        close();
        return;
    }

    // Set input read rate based on the default STK sample rate.
    double rate = 1.0;
    rate = input->getFileRate() / Stk::sampleRate();
    //rate *= 2.0; //uncomment to play back audio at twice the speed
    input->setRate(rate);
    qDebug() << "setting input file loop rate to:" << rate;

    //input->ignoreSampleRateChange();

    // Find out how many channels we have.
    int channels = input->channelsOut();

#if 0
    QList<QAudioDeviceInfo> allDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    //qDebug() << "all devices:" << allDevices;
    for(int i = 0; i < allDevices.size(); ++i)
    {
        const QAudioDeviceInfo &currentInfo = allDevices.at(i);
        qDebug() << "device #" << i << ":" << currentInfo.deviceName();
        qDebug() << "supported sample sizes:" << currentInfo.supportedSampleSizes();
        qDebug() << "supported endiannesses:" << currentInfo.supportedByteOrders();
        qDebug() << "";
    }
#endif

    QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(Stk::sampleRate());
    audioFormat.setChannelCount(channels);
    audioFormat.setSampleSize(32);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::Float);

    if(!audioDeviceInfo.isFormatSupported(audioFormat))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        audioFormat = audioDeviceInfo.nearestFormat(audioFormat);
    }
    QAudioOutput *audioOutput = new QAudioOutput(audioDeviceInfo, audioFormat, this);
    m_FileLoopIoDevice = new StkFileLoopIoDevice(input.take(), channels, audioOutput);

    m_FileLoopIoDevice->reserveFrames(m_NumBufferFrames, channels);
    m_FileLoopIoDevice->start();
    audioOutput->start(m_FileLoopIoDevice);
}
qreal LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::map(qreal valueToMap, qreal sourceRangeMin, qreal sourceRangeMax, qreal destRangeMin, qreal destRangeMax)
{
    //map 'valueToMap' from [sourceRangeMin-sourceRangeMax] to [destRangeMin-destRangeMax]
    qreal mappedValue = (valueToMap - sourceRangeMin) * (destRangeMax - destRangeMin) / (sourceRangeMax - sourceRangeMin) + destRangeMin; //jacked from Arduino/WMath.cpp , which in turn was jacked from Wiring. it's just math anyways (not copyrightable)
    return mappedValue;
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::showStdStringError(const std::string &stkError)
{
    QString stkErrorQString = QString::fromStdString(stkError);
    QMessageBox::critical(this, "StkError:", stkErrorQString);
    close();
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::setupGui()
{
    QHBoxLayout *myLayout = new QHBoxLayout(this);

    myLayout->addWidget(m_PitchShiftSlider = new QSlider());
    m_PitchShiftSlider->setRange(0, 1023);
    m_PitchShiftSlider->setValue((m_PitchShiftSlider->minimum() + m_PitchShiftSlider->maximum()) / 2);
    connect(m_PitchShiftSlider, &QSlider::valueChanged, this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftSliderValueChanged);

    myLayout->addWidget(m_PitchShiftMixAmountSlider = new QSlider());
    m_PitchShiftMixAmountSlider->setRange(0, 100);
    m_PitchShiftMixAmountSlider->setValue((m_PitchShiftMixAmountSlider->minimum() + m_PitchShiftMixAmountSlider->maximum()) / 2);
    connect(m_PitchShiftMixAmountSlider, &QSlider::valueChanged, this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftMixAmountSliderValueChanged);
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftSliderValueChanged(int newValue)
{
    //bleh all the PitShift docs say is that "1.0" has no pitch shift effect, doesn't tell me which direction to go or what range min/max I should map my 0-1023 values to. bah I guess trial and error (but if I don't change it ENOUGH, I won't hear shit!)
    qreal newValueAsFloat = static_cast<qreal>(newValue);

    //map 0-1023 -> 0.1-2
    qreal newValueMapped = map(newValueAsFloat, 0, 1023, 0.1, 2);

    m_FileLoopIoDevice->setPitchShift(static_cast<StkFloat>(newValueMapped)); //TODOreq: this might not be thread-safe, idfk. maybe I need to "post a message to stk thread"? they do have a Messenger thing goin on fuck idk
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftMixAmountSliderValueChanged(int newValue)
{
    qreal newValueAsFloat = static_cast<qreal>(newValue);

    //map 0-100 -> 0-1
    qreal newValueMapped = map(newValueAsFloat, 0, 100, 0, 1);

    m_FileLoopIoDevice->setPitchShiftMixAmount(static_cast<StkFloat>(newValueMapped));
}
