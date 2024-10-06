#include "submodulecloner.h"

#include "settings.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/log.h>
#include <QRandomGenerator>

using namespace GIT;

SubmoduleCloner::SubmoduleCloner(GIT::Repository* superRepo) :
    QObject(),
    _superRepo(superRepo)
{
}

SubmoduleCloner::~SubmoduleCloner()
{
    qDeleteAll(_threads);
}

void SubmoduleCloner::start()
{
    for(const Submodule& submodule : _superRepo->submodules()) {
        if(submodule.isWorkdirInitialized()) {
            continue;
        }
        SubmoduleUpdateThread* thread = new SubmoduleUpdateThread(this, submodule);
        connect(thread, &SubmoduleUpdateThread::progress, this, &SubmoduleCloner::onThreadProgress);
        connect(thread, &SubmoduleUpdateThread::finished, this, &SubmoduleCloner::onThreadFinished);
        _threads.insert(submodule.name(), thread);
        thread->start();

        emit submoduleStarted(submodule.name());
    }
}

void SubmoduleCloner::onThreadProgress(double percent)
{
    SubmoduleUpdateThread* thread = static_cast<SubmoduleUpdateThread*>(sender());
    emit progress(thread->submodule().name(), percent * 100);
}

void SubmoduleCloner::onThreadFinished()
{
    SubmoduleUpdateThread* thread = static_cast<SubmoduleUpdateThread*>(sender());
    emit submoduleFinished(thread->submodule().name());
    int finishedCount = std::count_if(_threads.constBegin(), _threads.constEnd(), [](SubmoduleUpdateThread* t) { return t->isFinished(); });
    Log::logText(LVL_DEBUG, QString("%1 finished (%2 of %3)").arg(thread->submodule().name()).arg(finishedCount).arg(_threads.count()));
    if(finishedCount == _threads.count()) {
        emit finished();
    }
}


// --------------------------- SubmoduleUpdateThread ---------------------------


SubmoduleUpdateThread::SubmoduleUpdateThread(SubmoduleCloner* parent, const GIT::Submodule& submodule) :
    _parent(parent), _superRepo(nullptr), _submodule(submodule)
{
    SubmoduleUpdateThread::setObjectName(QString("%1 (%2)").arg(SubmoduleUpdateThread::metaObject()->className()).arg(submodule.name()));
    _credentialResolver.setCredentials(Settings::instance()->repoConfig(parent->superRepo()).credentials());

    connect(&_progressCallback, &CloneProgressCallback::progress, this, &SubmoduleUpdateThread::onProgress);
}

SubmoduleUpdateThread::~SubmoduleUpdateThread()
{
    delete _superRepo;
}

void SubmoduleUpdateThread::threadStarted()
{
    try
    {
        _superRepo = new Repository(_parent->superRepo()->localPath());
        _submodule = _superRepo->submodules().findByName(_submodule.name());
        if(_submodule.isNull()) {
            throw CommonException("Failed to find submodule");
        }

        // Retry for up to 1000msec in the case of file-lock issues
        static const TimeSpan RetryTime = TimeSpan::fromMilliseconds(1000);
        QDateTime start = QDateTime::currentDateTimeUtc();
        git_error_code err;
        do
        {
            if(_submodule.update(true, &_credentialResolver, &_progressCallback) == true) {
                break;
            }

            err = _superRepo->errorCode();
            if(err == GIT_ELOCKED) {
                int msecs = QRandomGenerator::global()->generate() % 50;
                logText(LVL_DEBUG, QString("Repo is locked... will sleep %1").arg(msecs));
                QThread::msleep(msecs);
            }
            else {
                throw CommonException(_superRepo->errorText());
            }
        }while(err == GIT_ELOCKED && TimeSpan::absDiff(QDateTime::currentDateTimeUtc(), start) < RetryTime);

        finishAndStop(true, "Success");
    }
    catch(const CommonException& e)
    {
        logText(LVL_WARNING, e.message());
        finishAndStop(false, e.message());
    }
}

void SubmoduleUpdateThread::threadFinished()
{
    _finished = true;
}

void SubmoduleUpdateThread::onProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    Q_UNUSED(receivedBytes)

    double percent = (double)receivedObjects / (double)totalObjects;
    emit progress(percent);
}
