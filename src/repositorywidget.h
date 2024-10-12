#ifndef REPOSITORY_WIDGET_H
#define REPOSITORY_WIDGET_H

#include "repoconfig.h"

#include <Kanoop/gui/complexwidget.h>
#include <cloneprogresscallback.h>
#include <git2qt.h>
#include <gitcredentialresolver.h>
#include <submoduleupdateprogresscallback.h>
#include <kanoopgittypes.h>


class ToastManager;
namespace Ui {
class RepositoryWidget;
}

namespace GIT {
class Repository;
}
class RepositoryWidget : public ComplexWidget
{
    Q_OBJECT

public:
    explicit RepositoryWidget(GIT::Repository* repo, QWidget *parent = nullptr);
    ~RepositoryWidget();

    GIT::Repository* repository() const { return _repo; }

    bool isOnStash() const;

signals:
    void validate();
    void submoduleDoubleClicked(const GIT::Submodule& submodule);

public slots:
    void pullFromRemote();
    void pushToRemote();
    void beginCreateBranch();
    void stashChanges();
    void doDebugThing();
    void popStash();
    void showRepoOptionsDialog();

private:
    void initializeCredentials();
    void createToastContainer();
    void updateCommitShaWidget(const GIT::ObjectId& objectId);
    void updateParentsShaWidget(const GIT::ObjectId& objectId);
    void updateParentsShaWidget(const GIT::ObjectId::List& objectIds);
    void stageFile(const GIT::StatusEntry& entry, const GIT::DiffDelta& delta);
    void unstageFile(const GIT::StatusEntry& entry, const GIT::DiffDelta& delta);
    void showLocalBranchCustomContextMenu(const GIT::Reference& reference);
    void showSubmoduleCustomContextMenu(const GIT::Submodule& submodule);
    void showSubmodulesCustomContextMenu();

    // QWidget interface
    virtual void resizeEvent(QResizeEvent* event) override;

    Ui::RepositoryWidget *ui;
    GIT::Repository* _repo;
    RepoConfig _config;
    bool _filesystemWatchEnabled = true;
    ToastManager* _toastManager = nullptr;
    RefreshItems _refreshItems = RefreshNone;

    GitCredentialResolver _credentialResolver;
    SubmoduleUpdateProgressCallback _submoduleUpdateProgressCallback;

    static const QString StageUnstageProperty;
    static const QString ReferenceProperty;
    static const QString SubmoduleProperty;
    static const int ToastWidth = 300;

    enum StageType { StageTypeInvalid, StageFile, UnstageFile };

private slots:
    void refreshWidgets(RefreshItems refreshItems);
    void refreshStatusEntries();
    void maybeEnableButtons();
    void onRepositoryFileSystemChanged();
    void switchToDiffView();
    void switchToCommitView();
    void suspendWatchingFileSystem() { _filesystemWatchEnabled = false; }
    void resumeWatchingFileSystem() { _filesystemWatchEnabled = true; }

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
    void onRemoteReferenceDoubleClicked(const GIT::Reference& reference);
    void onCommitShaClicked(const GIT::ObjectId& objectId);
    void onStagedStatusEntryClicked(const GIT::StatusEntry& statusEntry);
    void onUnstagedStatusEntryClicked(const GIT::StatusEntry& statusEntry);
    void onTreeChangeEntryClicked(const GIT::TreeChangeEntry& treeChangeEntry);
    void onSubmoduleClicked(const GIT::Submodule& submodule);

    // Context Menus
    void onStagedFilesContextMenuRequested();
    void onUnstagedFilesContextMenuRequested();
    void onCommitTableContextMenuRequested(const QPoint &pos);
    void onLeftSidebarCustomContextMenuRequested(const QPoint &pos);

    // Actions
    void onStageFilesClicked();
    void onUnstageFilesClicked();
    void onIgnoreClicked();
    void onDiscardChangesClicked();
    void onApplyStashClicked();
    void onDeleteStashClicked();
    void onDeleteLocalBranchClicked();
    void onRenameLocalBranchClicked();
    void onInitializeSubmoduleClicked();
    void onDeleteSubmoduleClicked();
    void onInitializeAllSubmodulesClicked();
    void onAddSubmoduleClicked();

    // Pushbuttons
    void onStageAllChangesClicked();
    void onStageDiffFileClicked();
    void onUnstageAllChangesClicked();
    void onCommitChangesClicked();

    void onNextDiffClicked();
    void onPreviousDiffClicked();

    // Callbacks
    void onSubmoduleUpdateProgress(double percent);
    void onSubmoduleUpdateProgress2(const QString& name, double percent);
    void onSubmoduleUpdateStarted(const QString& name);
    void onSubmoduleUpdateFinished(const QString& name);
    void onSubmoduleUpdaterFinished();

    // Debug
    void drawDebugArc();
};

#endif // REPOSITORY_WIDGET_H
