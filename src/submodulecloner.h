#ifndef SUBMODULECLONER_H
#define SUBMODULECLONER_H
#include <Kanoop/utility/abstractthreadclass.h>

#include <git2qt.h>
#include <gitcredentialresolver.h>

#include "submoduleupdateprogresscallback.h"

class SubmoduleUpdateThread;
class SubmoduleCloner : public QObject
{
    Q_OBJECT
public:
    SubmoduleCloner(GIT::Repository* superRepo);
    virtual ~SubmoduleCloner();

    void start();

    GIT::Repository* superRepo() const { return _superRepo; }

private:
    GIT::Repository* _superRepo;

    QMap<QString, SubmoduleUpdateThread*> _threads;

signals:
    void progress(const QString& submoduleName, double percent);
    void submoduleStarted(const QString& submoduleName);
    void submoduleFinished(const QString& submoduleName);
    void finished();

private slots:
    void onThreadProgress(double percent);
    void onThreadFinished();
};

class SubmoduleUpdateThread : public AbstractThreadClass
{
    Q_OBJECT
public:
    SubmoduleUpdateThread(SubmoduleCloner* parent, const GIT::Submodule& submodule);
    virtual ~SubmoduleUpdateThread();

    GIT::Submodule submodule() const { return _submodule; }
    bool isFinished() const { return _finished; }

private:
    virtual void threadStarted() override;
    virtual void threadFinished() override;

    SubmoduleCloner* _parent;
    GIT::Repository* _superRepo;
    GIT::Submodule _submodule;
    bool _finished = false;

    SubmoduleUpdateProgressCallback _progressCallback;
    GitCredentialResolver _credentialResolver;

signals:
    void progress(double percent);

private slots:
    void onProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // SUBMODULECLONER_H
