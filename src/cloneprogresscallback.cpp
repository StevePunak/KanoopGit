#include "cloneprogresscallback.h"


CloneProgressCallback::CloneProgressCallback(QObject* parent) :
    QObject(parent)
{

}

void CloneProgressCallback::progressCallback(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    emit progress(receivedBytes, receivedObjects, totalObjects);
}
