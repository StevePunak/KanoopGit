#ifndef SUBMODULEUPDATEPROGRESSCALLBACK_H
#define SUBMODULEUPDATEPROGRESSCALLBACK_H
#include "cloneprogresscallback.h"
#include <git2qt.h>

class SubmoduleUpdateProgressCallback : public CloneProgressCallback
{
    Q_OBJECT
public:
    SubmoduleUpdateProgressCallback(QObject* parent = nullptr) :
        CloneProgressCallback(parent) {}

    GIT::Submodule submodule() const { return _submodule; }
    void setSubmodule(const GIT::Submodule& value) { _submodule = value; }

private:
    GIT::Submodule _submodule;
};

#endif // SUBMODULEUPDATEPROGRESSCALLBACK_H
