#ifndef GITMAINWINDOW_H
#define GITMAINWINDOW_H

#include <Kanoop/gui/mainwindowbase.h>
#include <cloneprogresscallback.h>

namespace Ui {
class GitMainWindow;
}

class RepositoryContainer;
class GitMainWindow : public MainWindowBase
{
    Q_OBJECT

public:
    explicit GitMainWindow(QWidget *parent = nullptr);
    ~GitMainWindow();

private:
    void openRecentRepos();
    RepositoryContainer* openRepository(const QString& path);
    void closeRepository(int tabIndex);

    Ui::GitMainWindow *ui;

    CloneProgressCallback _progressCallback;

signals:
    void loaded();

private slots:
    // Actions
    void onCloneRepoClicked();
    void onOpenRepoClicked();
    void onCloseTabClicked();
    void onCopyUrlToClipboard();
    void onCopyPathToClipboard();
    void onPreferencesClicked();

    // Double-clicks
    void onRecentRepoDoubleClicked(const QModelIndex &index);

    // Context Menus
    void onTabBarContextMenuRequested(int index);

    // Tab bar handlers
    void onTabCloseRequested(int index);
    void onTabBarClicked(int index);

    // Other
    void onCloneProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // GITMAINWINDOW_H
