#include "testwidgetsettings.h"

#include <QMutex>

const QString TestWidgetSettings::KEY_OPEN_REPOS        = "open_repos";
const QString TestWidgetSettings::KEY_RECENT_FILES      = "recent_files";

TestWidgetSettings* TestWidgetSettings::instance()
{
    static TestWidgetSettings* _instance = nullptr;
    static QMutex _startLock;

    _startLock.lock();
    if(_instance == nullptr) {
        _instance = new TestWidgetSettings;
    }
    _startLock.unlock();
    return _instance;
}

TestWidgetSettings::TestWidgetSettings() :
    GuiSettings()
{
    setGlobalInstance(this);
}

void TestWidgetSettings::saveOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    if(repos.contains(value) == false) {
        repos.insert(0, value);
        _settings.setValue(KEY_OPEN_REPOS, repos);
    }
}

void TestWidgetSettings::removeOpenRepo(const QString& value)
{
    QStringList repos = openRepos();
    repos.removeAll(value);
    _settings.setValue(KEY_OPEN_REPOS, repos);
}

void TestWidgetSettings::pushRecentFile(const QString &value)
{
    QStringList files = recentFiles();
    files.removeAll(value);
    files.insert(0, value);
}
