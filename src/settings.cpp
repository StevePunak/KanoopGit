#include "settings.h"

#include "repoconfig.h"

#include <QMutex>

const QString Settings::KEY_ACTIVE_REPO                 = "active_repo";
const QString Settings::KEY_CREDENTIALS                 = "creds";
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
    if(repos.contains(value) == false) {
        repos.insert(0, value);
        _settings.setValue(KEY_OPEN_REPOS, repos);
    }
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

void Settings::saveRepoConfig(const RepoConfig& config)
{
    QString key = makeRepoConfigKey(config.repoPath());
    _settings.setValue(key, config.toVariant());
}

RepoConfig Settings::repoConfig(const QString& repoPath) const
{
    RepoConfig config;
    QString key = makeRepoConfigKey(repoPath);
    QVariant value = _settings.value(key);
    if(value.isNull() == false) {
        config = RepoConfig::fromVariant(value);
        if(config.isValid() == false) {
            config.setRepoPath(repoPath);
        }
    }
    return config;
}

void Settings::ensureValidDefaults()
{
    int points = fontSize();
    if(points == 0) {
        setFontSize(11);
    }
}

QString Settings::makeRepoConfigKey(const QString& name)
{
    QString squashed = name;
    squashed.replace("/", "");
    squashed.replace("\\", "");
    return QString("repo_config/%1").arg(squashed);
}
