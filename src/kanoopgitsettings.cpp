#include "kanoopgitsettings.h"

#include <QMutex>

const QString KanoopGitSettings::KEY_OPEN_REPOS        = "open_repos";
const QString KanoopGitSettings::KEY_RECENT_FILES      = "recent_files";

KanoopGitSettings* KanoopGitSettings::instance()
{
    static KanoopGitSettings* _instance = nullptr;
    static QMutex _startLock;

    _startLock.lock();
    if(_instance == nullptr) {
        _instance = new KanoopGitSettings;
        _instance->ensureValidDefaults();
    }
    _startLock.unlock();
    return _instance;
}

KanoopGitSettings::KanoopGitSettings() :
    GuiSettings()
{
    setGlobalInstance(this);
}

void KanoopGitSettings::saveOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    if(repos.contains(value) == false) {
        repos.insert(0, value);
        _settings.setValue(KEY_OPEN_REPOS, repos);
    }
}

void KanoopGitSettings::removeOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    repos.removeAll(value);
    _settings.setValue(KEY_OPEN_REPOS, repos);
}

void KanoopGitSettings::pushRecentFile(const QString &value)
{
    QStringList files = recentFiles();
    files.removeAll(value);
    files.insert(0, value);
}

void KanoopGitSettings::ensureValidDefaults()
{
    int points = fontSize();
    if(points == 0) {
        setFontSize(11);
    }
}
