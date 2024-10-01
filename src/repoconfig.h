#ifndef REPOCONFIG_H
#define REPOCONFIG_H
#include "credentialset.h"

#include <QVariant>

class RepoConfig
{
public:
    RepoConfig() {}

    QString repoPath() const { return _repoPath; }
    void setRepoPath(const QString& value) { _repoPath = value; }

    bool localBranchesVisible() const { return _localBranchesVisible; }
    void setLocalBranchesVisible(bool value) { _localBranchesVisible = value; }

    bool remoteBranchesVisible() const { return _remoteBranchesVisible; }
    void setRemoteBranchesVisible(bool value) { _remoteBranchesVisible = value; }

    CredentialSet& credentialsRef() { return _credentials; }
    void setCredentials(CredentialSet value) { _credentials = value; }

    QVariant toVariant() const { return QVariant::fromValue<RepoConfig>(*this); }
    static RepoConfig fromVariant(const QVariant& value) { return value.value<RepoConfig>(); }

    void fromDataStream(QDataStream& in);
    void toDataStream(QDataStream& out) const;

    bool isValid() const { return _repoPath.isEmpty() == false; }

private:
    QString _repoPath;
    bool _localBranchesVisible = false;
    bool _remoteBranchesVisible = false;
    CredentialSet _credentials;
};

QDataStream &operator<<(QDataStream &out, const RepoConfig &config);
QDataStream &operator>>(QDataStream &in, RepoConfig &config);

Q_DECLARE_METATYPE(RepoConfig)

#endif // REPOCONFIG_H
