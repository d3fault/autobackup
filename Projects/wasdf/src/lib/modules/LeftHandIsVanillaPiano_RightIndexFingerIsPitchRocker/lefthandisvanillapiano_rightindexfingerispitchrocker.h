#ifndef LEFTHANDISVANILLAPIANO_RIGHTINDEXFINGERISPITCHROCKER_H
#define LEFTHANDISVANILLAPIANO_RIGHTINDEXFINGERISPITCHROCKER_H

#include <QObject>
#include "../../iwasdfmodule.h"
#include "../iwasdfmodulebasicaudioclipsampler.h"

#include "../../wasdf.h"

class LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker : public QObject, public IWasdfModule, public IWasdfModuleBasicAudioClipSampler
{
    Q_OBJECT
public:
    explicit LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker(Wasdf *wasdf, QObject *parent = 0);
    virtual ~LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker() = default;

    virtual QObject *asQObject() const override { return this; }
public: //slots:
    void handleFingerMoved(Finger fingerThatMoved, int newPos) override;
};

#endif // LEFTHANDISVANILLAPIANO_RIGHTINDEXFINGERISPITCHROCKER_H
