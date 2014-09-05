#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTSERIALIZER_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTSERIALIZER_H

#include <QObject>

class QIODevice;

class DesignEqualsImplementationProject;

class DesignEqualsImplementationProjectSerializer : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectSerializer(QObject *parent = 0);
public slots:
    void serializeProjectToIoDevice(DesignEqualsImplementationProject *projectToSerialize, QIODevice *ioDeviceToSerializeTo);
    void deserializeProjectFromIoDevice(QIODevice *ioDeviceToDeserializeFrom, DesignEqualsImplementationProject *projectToPopulate);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTSERIALIZER_H
