#ifndef SETTINGS_H
#define SETTINGS_H

#include <Kanoop/gui/guisettings.h>

#include <credentialset.h>


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

    bool areLocalBranchesVisible() const { return _settings.value(KEY_LOCAL_BRANCHES_VISIBLE).toBool(); }
    void setLocalBranchesVisible(bool value) { _settings.setValue(KEY_LOCAL_BRANCHES_VISIBLE, value); }

    bool areRemoteBranchesVisible() const { return _settings.value(KEY_REMOTE_BRANCHES_VISIBLE).toBool(); }
    void setRemoteBranchesVisible(bool value) { _settings.setValue(KEY_REMOTE_BRANCHES_VISIBLE, value); }

    bool areSubmodulesVisible() const { return _settings.value(KEY_SUBMODULES_VISIBLE).toBool(); }
    void setSubmodulesVisible(bool value) { _settings.setValue(KEY_SUBMODULES_VISIBLE, value); }


private:
    virtual void ensureValidDefaults() override;

    static const QString KEY_CREDENTIALS;
    static const QString KEY_LOCAL_BRANCHES_VISIBLE;
    static const QString KEY_REMOTE_BRANCHES_VISIBLE;
    static const QString KEY_SUBMODULES_VISIBLE;
    static const QString KEY_OPEN_REPOS;
    static const QString KEY_RECENT_FILES;
};

#endif // SETTINGS_H
