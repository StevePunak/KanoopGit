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
    void onCloneRepoClicked();
    void onOpenRepoClicked();
    void onCloseTabClicked();
    void onRecentRepoDoubleClicked(const QModelIndex &index);
    void onTabCloseRequested(int index);
    void onTabBarClicked(int index);
    void onPreferencesClicked();
    void onCloneProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // GITMAINWINDOW_H
