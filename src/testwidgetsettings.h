#ifndef TESTWIDGETSETTINGS_H
#define TESTWIDGETSETTINGS_H

#include <Kanoop/gui/guisettings.h>


class TestWidgetSettings : public GuiSettings
{
public:
    static TestWidgetSettings* instance();

    TestWidgetSettings();

    QStringList openRepos() const { return _settings.value(KEY_OPEN_REPOS).toStringList(); }
    void saveOpenRepo(const QString& value);
    void removeOpenRepo(const QString& value);

    QStringList recentFiles() const { return _settings.value(KEY_RECENT_FILES).toStringList(); }
    void pushRecentFile(const QString& value);

private:
    static const QString KEY_OPEN_REPOS;
    static const QString KEY_RECENT_FILES;
};

#endif // TESTWIDGETSETTINGS_H
