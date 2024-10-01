#ifndef REPOSITORY_WIDGET_H
#define REPOSITORY_WIDGET_H

#include "repoconfig.h"

#include <Kanoop/gui/mainwindowbase.h>
#include <git2qt.h>
#include <gitcredentialresolver.h>

namespace Ui {
class RepositoryWidget;
}

namespace GIT {
class Repository;
}
class RepositoryWidget : public MainWindowBase
{
    Q_OBJECT

public:
    explicit RepositoryWidget(const QString& path, QWidget *parent = nullptr);
    ~RepositoryWidget();

    GIT::Repository* repository() const { return _repo; }

private:
    void initializeCredentials();
    void initializeButtonLabels();
    void updateCommitShaWidget(const GIT::ObjectId& objectId);
    void updateParentsShaWidget(const GIT::ObjectId& objectId);
    void updateParentsShaWidget(const GIT::ObjectId::List& objectIds);
    void stageFile(const GIT::StatusEntry& entry, const GIT::DiffDelta& delta);
    void unstageFile(const GIT::StatusEntry& entry, const GIT::DiffDelta& delta);

    Ui::RepositoryWidget *ui;
    GIT::Repository* _repo;
    GitCredentialResolver _credentialResolver;
    RepoConfig _config;

    static const QString StageUnstageProperty;
    static const QString ReferenceProperty;

    enum StageType { StageTypeInvalid, StageFile, UnstageFile };

private slots:
    void refreshWidgets();
    void maybeEnableButtons();
    void onRepositoryFileSystemChanged();
    void switchToDiffView();
    void switchToCommitView();

    // Refresh
    void onRefreshWidgets();

    // Slots for views
    void createBranch(const QString& branchName);

    // Widget overrides
    virtual void keyPressEvent(QKeyEvent* event) override;

    // Widget clicks
    void onFolderClicked(const QString& folderPath);
    void onIndexEntryClicked(const GIT::IndexEntry& indexEntry);
    void onCommitClicked(const GIT::GraphedCommit& commit);
    void onStashCommitClicked(const GIT::Stash& stash);
    void onWorkInProgressClicked();
    void onReferenceClicked(const GIT::Reference& reference);
    void onLocalReferenceDoubleClicked(const GIT::Reference& reference);
    void onCommitShaClicked(const GIT::ObjectId& objectId);
    void onStagedStatusEntryClicked(const GIT::StatusEntry& statusEntry);
    void onUnstagedStatusEntryClicked(const GIT::StatusEntry& statusEntry);
    void onTreeChangeEntryClicked(const GIT::TreeChangeEntry& treeChangeEntry);

    // Context Menus
    void onStagedFilesContextMenuRequested();
    void onUnstagedFilesContextMenuRequested();
    void onCommitTableContextMenuRequested(const QPoint &pos);
    void onLocalBranchesCustomContextMenuRequested(const QPoint &pos);

    // Actions
    void onStageFilesClicked();
    void onUnstageFilesClicked();
    void onIgnoreClicked();
    void onStashChangesClicked();
    void onDiscardChangesClicked();
    void onApplyStashClicked();
    void onDeleteStashClicked();
    void onPopStashClicked();
    void onDeleteLocalBranchClicked();
    void onRenameLocalBranchClicked();

    // Pushbuttons
    void onStageAllChangesClicked();
    void onStageDiffFileClicked();
    void onUnstageAllChangesClicked();
    void onCommitChangesClicked();
    void onPullFromRemoteClicked();
    void onPushToRemoteClicked();
    void onCreateBranchClicked();
    void onNextDiffClicked();
    void onPreviousDiffClicked();
    void onDebugClicked();

    // Debug
    void drawDebugArc();
};

#endif // REPOSITORY_WIDGET_H
