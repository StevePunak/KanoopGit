#include "repoconfig.h"


void RepoConfig::fromDataStream(QDataStream& in)
{
    in >> _repoPath
            >> _localBranchesVisible
            >> _remoteBranchesVisible
            >> _submodulesVisible
            >> _credentials
            >> _openedSubmodule;
}

void RepoConfig::toDataStream(QDataStream& out) const
{
    out << _repoPath
        << _localBranchesVisible
        << _remoteBranchesVisible
        << _submodulesVisible
        << _credentials
        << _openedSubmodule;
}

QDataStream& operator<<(QDataStream& out, const RepoConfig& config)
{
    config.toDataStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, RepoConfig& config)
{
    config.fromDataStream(in);
    return in;
}

