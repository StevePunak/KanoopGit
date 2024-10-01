#include "settings.h"

#include <QMutex>

const QString Settings::KEY_CREDENTIALS                 = "creds";
const QString Settings::KEY_LOCAL_BRANCHES_VISIBLE      = "local_branches_visible";
const QString Settings::KEY_REMOTE_BRANCHES_VISIBLE     = "remote_branches_visible";
const QString Settings::KEY_SUBMODULES_VISIBLE          = "submodules_visible";
const QString Settings::KEY_OPEN_REPOS                  = "open_repos";
const QString Settings::KEY_RECENT_FILES                = "recent_files";

Settings* Settings::instance()
{
    static Settings* _instance = nullptr;
    static QMutex _startLock;

    _startLock.lock();
    if(_instance == nullptr) {
        _instance = new Settings;
        _instance->ensureValidDefaults();
    }
    _startLock.unlock();
    return _instance;
}

Settings::Settings() :
    GuiSettings()
{
    setGlobalInstance(this);
}

void Settings::saveOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    repos.removeAll(value);
    repos.insert(0, value);
    _settings.setValue(KEY_OPEN_REPOS, repos);
}

void Settings::removeOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    repos.removeAll(value);
    _settings.setValue(KEY_OPEN_REPOS, repos);
}

void Settings::pushRecentFile(const QString &value)
{
    QStringList files = recentFiles();
    files.removeAll(value);
    files.insert(0, value);
    _settings.setValue(KEY_RECENT_FILES, files);
}

void Settings::saveCredentials(CredentialSet& credentialSet)
{
    QByteArray json = _settings.value(KEY_CREDENTIALS).toByteArray();
    CredentialSet::List credentials;
    credentials.deserializeFromJson(json);
    credentials.saveCredentials(credentialSet);
    json = credentials.serializeToJson();
    _settings.setValue(KEY_CREDENTIALS, json);
}

CredentialSet Settings::credentials(const QString& name) const
{
    QByteArray json = _settings.value(KEY_CREDENTIALS).toByteArray();
    CredentialSet::List credentials;
    credentials.deserializeFromJson(json);
    return credentials.findByName(name);
}

CredentialSet Settings::defaultCredentials() const
{
    return credentials(CredentialSet::DefaultName);
}

void Settings::ensureValidDefaults()
{
    int points = fontSize();
    if(points == 0) {
        setFontSize(11);
    }
}
