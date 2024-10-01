#ifndef CLONEPROGRESSCALLBACK_H
#define CLONEPROGRESSCALLBACK_H
#include <git2qt.h>
#include <QObject>

class CloneProgressCallback : public QObject,
                              public GIT::ProgressCallback

{
    Q_OBJECT
public:
    explicit CloneProgressCallback(QObject* parent = nullptr);

    virtual void progressCallback(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects) override;

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // CLONEPROGRESSCALLBACK_H
