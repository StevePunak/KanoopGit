#ifndef GITMAINWINDOW_H
#define GITMAINWINDOW_H

#include <Kanoop/gui/mainwindowbase.h>

namespace Ui {
class GitMainWindow;
}

class RepositoryWidget;
class GitMainWindow : public MainWindowBase
{
    Q_OBJECT

public:
    explicit GitMainWindow(QWidget *parent = nullptr);
    ~GitMainWindow();

private:
    void openRecentRepos();
    RepositoryWidget* openRepository(const QString& path);
    void closeRepository(int tabIndex);

    Ui::GitMainWindow *ui;

private slots:
    void onCloneRepoClicked();
    void onOpenRepoClicked();
    void onCloseTabClicked();
    void onRecentRepoDoubleClicked(const QModelIndex &index);
    void onTabCloseRequested(int index);
    void onTabBarClicked(int index);
};

#endif // GITMAINWINDOW_H
