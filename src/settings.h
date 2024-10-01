#ifndef SETTINGS_H
#define SETTINGS_H

#include <Kanoop/gui/guisettings.h>

#include <credentialset.h>

class RepoConfig;

class Settings : public GuiSettings
{
public:
    static Settings* instance();

    Settings();

    QStringList openRepos() const { return _settings.value(KEY_OPEN_REPOS).toStringList(); }
    void saveOpenRepo(const QString& value);
    void removeOpenRepo(const QString& value);

    QStringList recentFiles() const { return _settings.value(KEY_RECENT_FILES).toStringList(); }
    void pushRecentFile(const QString& value);

    void saveCredentials(CredentialSet& credentialSet);
    CredentialSet credentials(const QString& name) const;
    CredentialSet defaultCredentials() const;

    void saveRepoConfig(const RepoConfig& config);
    RepoConfig repoConfig(const QString& repoPath) const;

private:
    virtual void ensureValidDefaults() override;

    static QString makeRepoConfigKey(const QString& name);

    static const QString KEY_CREDENTIALS;
    static const QString KEY_OPEN_REPOS;
    static const QString KEY_RECENT_FILES;
};

#endif // SETTINGS_H
