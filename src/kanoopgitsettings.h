#ifndef KANOOPGITSETTINGS_H
#define KANOOPGITSETTINGS_H

#include <Kanoop/gui/guisettings.h>


class KanoopGitSettings : public GuiSettings
{
public:
    static KanoopGitSettings* instance();

    KanoopGitSettings();

    QStringList openRepos() const { return _settings.value(KEY_OPEN_REPOS).toStringList(); }
    void saveOpenRepo(const QString& value);
    void removeOpenRepo(const QString& value);

    QStringList recentFiles() const { return _settings.value(KEY_RECENT_FILES).toStringList(); }
    void pushRecentFile(const QString& value);

private:
    virtual void ensureValidDefaults() override;

    static const QString KEY_OPEN_REPOS;
    static const QString KEY_RECENT_FILES;
};

#endif // KANOOPGITSETTINGS_H
