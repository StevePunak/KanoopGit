#ifndef SETTINGS_H
#define SETTINGS_H

#include "repoconfig.h"

#include <Kanoop/gui/guisettings.h>
#include <git2qt.h>
#include <credentialset.h>
#include <kanoopgittypes.h>

class Settings : public GuiSettings
{
public:
    static Settings* instance();

    Settings();

    QStringList openRepos() const { return _settings.value(KEY_OPEN_REPOS).toStringList(); }
    void saveOpenRepo(const QString& value);
    void removeOpenRepo(const QString& value);

    QString activeRepo() const { return _settings.value(KEY_ACTIVE_REPO).toString(); }
    void saveActiveRepo(const QString& value) { _settings.setValue(KEY_ACTIVE_REPO, value); }

    QStringList recentFiles() const { return _settings.value(KEY_RECENT_FILES).toStringList(); }
    void pushRecentFile(const QString& value);

    void saveCredentials(CredentialSet& credentialSet);
    CredentialSet credentials(const QString& name) const;
    CredentialSet defaultCredentials() const;

    void saveRepoConfig(const RepoConfig& config);
    RepoConfig repoConfig(GIT::Repository* repo) const { return repoConfig(repo->localPath()); }
    RepoConfig repoConfig(const QString& repoPath) const;

    void setPaletteType(PaletteType value) { _settings.setValue(KEY_PALETTE_TYPE, value); }
    PaletteType paletteType() const { return (PaletteType)_settings.value(KEY_PALETTE_TYPE).toInt(); }

    bool isDarkMode() const;

private:
    virtual void ensureValidDefaults() override;

    static QString makeRepoConfigKey(const QString& name);

    static const QString KEY_ACTIVE_REPO;
    static const QString KEY_CREDENTIALS;
    static const QString KEY_OPEN_REPOS;
    static const QString KEY_PALETTE_TYPE;
    static const QString KEY_RECENT_FILES;
};

#endif // SETTINGS_H
