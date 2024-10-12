#include "repositorywidget.h"
#include "parentsshawidget.h"
#include "shawidget.h"
#include "ui_repositorywidget.h"

#include <QMessageBox>
#include <QPainter>
#include <QMenu>
#include <git2qt.h>

#include "settings.h"
#include "kanoopgittypes.h"
#include "gitassets.h"
#include "submodulecloner.h"

#include <Kanoop/geometry/rectangle.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/pathutil.h>
#include <Kanoop/stringutil.h>

#include <Kanoop/gui/resources.h>

#include <dialogs/clonerepodialog.h>
#include <dialogs/repooptionsdialog.h>

#include <Kanoop/gui/widgets/toastmanager.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

const QString RepositoryWidget::StageUnstageProperty    = "stage_unstage";
const QString RepositoryWidget::ReferenceProperty       = "reference";
const QString RepositoryWidget::SubmoduleProperty       = "submodule";

RepositoryWidget::RepositoryWidget(Repository* repo, QWidget *parent) :
    ComplexWidget("gittree", parent),
    ui(new Ui::RepositoryWidget),
    _repo(repo)
{
    RepositoryWidget::setObjectName(RepositoryWidget::metaObject()->className());

    ui->setupUi(this);

    initializeBase();

    QApplication::setOverrideCursor(Qt::WaitCursor);

Log::logText(LVL_DEBUG, QString("%1   5-1").arg(__FUNCTION__));

    // Some initial values
    ui->textDiffFileName->clear();

    // Wire up views to this widget
    connect(ui->treeFileSystem, &FileSystemTreeView::indexEntryClicked, this, &RepositoryWidget::onIndexEntryClicked);
    connect(ui->treeFileSystem, &FileSystemTreeView::folderClicked, this, &RepositoryWidget::onFolderClicked);
    connect(ui->tableCommits, &CommitTableView::commitClicked, this, &RepositoryWidget::onCommitClicked);
    connect(ui->tableCommits, &CommitTableView::stashClicked, this, &RepositoryWidget::onStashCommitClicked);
    connect(ui->tableCommits, &CommitTableView::workInProgressClicked, this, &RepositoryWidget::onWorkInProgressClicked);
    connect(ui->tableCommits, &CommitTableView::currentSelectionChanged, this, &RepositoryWidget::maybeEnableButtons);
    connect(ui->tableCommits, &CommitTableView::createBranch, this, &RepositoryWidget::createBranch);
    connect(ui->leftSidebar, &LeftSidebarTreeView::referenceClicked, this, &RepositoryWidget::onReferenceClicked);
    connect(ui->leftSidebar, &LeftSidebarTreeView::localReferenceDoubleClicked, this, &RepositoryWidget::onLocalReferenceDoubleClicked);
    connect(ui->leftSidebar, &LeftSidebarTreeView::remoteReferenceDoubleClicked, this, &RepositoryWidget::onRemoteReferenceDoubleClicked);
    connect(ui->tableStagedFiles, &StatusEntryTableView::statusEntryClicked, this, &RepositoryWidget::onStagedStatusEntryClicked);
    connect(ui->tableUnstagedFiles, &StatusEntryTableView::statusEntryClicked, this, &RepositoryWidget::onUnstagedStatusEntryClicked);
    connect(ui->tableTreeEntries, &TreeEntryTableView::treeChangeEntryClicked, this, &RepositoryWidget::onTreeChangeEntryClicked);
    connect(ui->leftSidebar, &LeftSidebarTreeView::submoduleClicked, this, &RepositoryWidget::onSubmoduleClicked);
    connect(ui->leftSidebar, &LeftSidebarTreeView::submoduleDoubleClicked, this, &RepositoryWidget::submoduleDoubleClicked);

    // Context Menus
    connect(ui->tableStagedFiles, &StatusEntryTableView::customContextMenuRequested, this, &RepositoryWidget::onStagedFilesContextMenuRequested);
    connect(ui->tableUnstagedFiles, &StatusEntryTableView::customContextMenuRequested, this, &RepositoryWidget::onUnstagedFilesContextMenuRequested);
    connect(ui->tableCommits, &CommitTableView::customContextMenuRequested, this, &RepositoryWidget::onCommitTableContextMenuRequested);
    connect(ui->leftSidebar, &LeftSidebarTreeView::customContextMenuRequested, this, &RepositoryWidget::onLeftSidebarCustomContextMenuRequested);

    // Actions
    connect(ui->actionStageFiles, &QAction::triggered, this, &RepositoryWidget::onStageFilesClicked);
    connect(ui->actionUnstageFiles, &QAction::triggered, this, &RepositoryWidget::onUnstageFilesClicked);
    connect(ui->actionIgnore, &QAction::triggered, this, &RepositoryWidget::onIgnoreClicked);
    connect(ui->actionStash, &QAction::triggered, this, &RepositoryWidget::stashChanges);
    connect(ui->actionDiscardChanges, &QAction::triggered, this, &RepositoryWidget::onDiscardChangesClicked);
    connect(ui->actionApplyStash, &QAction::triggered, this, &RepositoryWidget::onApplyStashClicked);
    connect(ui->actionDeleteStash, &QAction::triggered, this, &RepositoryWidget::onDeleteStashClicked);
    connect(ui->actionPopStash, &QAction::triggered, this, &RepositoryWidget::popStash);
    connect(ui->actionDeleteLocalBranch, &QAction::triggered, this, &RepositoryWidget::onDeleteLocalBranchClicked);
    connect(ui->actionRenameLocalBranch, &QAction::triggered, this, &RepositoryWidget::onRenameLocalBranchClicked);
    connect(ui->actionInitializeSubmodule, &QAction::triggered, this, &RepositoryWidget::onInitializeSubmoduleClicked);
    connect(ui->actionDeleteSubmodule, &QAction::triggered, this, &RepositoryWidget::onDeleteSubmoduleClicked);
    connect(ui->actionInitializeAllSubmodules, &QAction::triggered, this, &RepositoryWidget::onInitializeAllSubmodulesClicked);
    connect(ui->actionAddSubmodule, &QAction::triggered, this, &RepositoryWidget::onAddSubmoduleClicked);

    // Pushbuttons
    connect(ui->pushStageAll, &QPushButton::clicked, this, &RepositoryWidget::onStageAllChangesClicked);
    connect(ui->pushStageDiffFile, &QPushButton::clicked, this, &RepositoryWidget::onStageDiffFileClicked);
    connect(ui->pushUnstageAll, &QPushButton::clicked, this, &RepositoryWidget::onUnstageAllChangesClicked);
    connect(ui->pushCommitChanges, &QPushButton::clicked, this, &RepositoryWidget::onCommitChangesClicked);
    connect(ui->pushNextDiff, &QToolButton::clicked, this, &RepositoryWidget::onNextDiffClicked);
    connect(ui->pushPreviousDiff, &QToolButton::clicked, this, &RepositoryWidget::onPreviousDiffClicked);
    connect(ui->pushCloseDiff, &QToolButton::clicked, this, &RepositoryWidget::switchToCommitView);

    // Debug stuff
    connect(ui->textStartAngle, &QLineEdit::textChanged, this, &RepositoryWidget::drawDebugArc);
    connect(ui->textSpanAngle, &QLineEdit::textChanged, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioDownToLeft, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioDownToRight, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioUpToLeft, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioUpToRight, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioLeftThenDown, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioLeftThenUp, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioRightThenDown, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);
    connect(ui->radioRightThenUp, &QRadioButton::toggled, this, &RepositoryWidget::drawDebugArc);

    // Repo wiring
    connect(_repo, &Repository::repositoryChanged, this, &RepositoryWidget::onRepositoryFileSystemChanged);

    // Callbacks
    connect(&_submoduleUpdateProgressCallback, &CloneProgressCallback::progressPercent, this, &RepositoryWidget::onSubmoduleUpdateProgress);

    // Validation wiring
    connect(ui->textStageCommitMessage, &QPlainTextEdit::textChanged, this, &RepositoryWidget::maybeEnableButtons);

    // Toast
    createToastContainer();

    _refreshItems = RefreshAll;
    onRefreshWidgets();

    // Set correct starting pages
    ui->stackedWidget->setCurrentWidget(ui->pageIndexEntry);
    ui->tabWidget->setCurrentWidget(ui->tabCommits);

Log::logText(LVL_DEBUG, QString("%1   5-3").arg(__FUNCTION__));
    // Set current commit
    if(ui->tableCommits->hasWorkInProgress()) {
        ui->tableCommits->selectWorkInProgress();
    }
    else if(_repo->headCommit().objectId().isValid()) {
        ui->tableCommits->selectCommit(_repo->headCommit().objectId());
        onCommitClicked(_repo->headCommit());
    }

    // Set up credentials resolver
    initializeCredentials();

    // Set button enablements and text
    maybeEnableButtons();
Log::logText(LVL_DEBUG, QString("%1   5-4").arg(__FUNCTION__));

QApplication::restoreOverrideCursor();

}

RepositoryWidget::~RepositoryWidget()
{
    delete ui;
}

bool RepositoryWidget::isOnStash() const
{
    return ui->tableCommits->currentMetadataType() == GitEntities::Stash;
}

void RepositoryWidget::initializeCredentials()
{
    // Try and find credentials for this repo (by sha of initial commit)
    _config = Settings::instance()->repoConfig(_repo);
    if(_config.isValid() == false) {
        _config.setRepoPath(_repo->localPath());
        _config.setCredentials(Settings::instance()->defaultCredentials());
        Settings::instance()->saveRepoConfig(_config);
    }
    _credentialResolver.setCredentials(_config.credentials());
    _repo->setCredentialResolver(&_credentialResolver);
}

void RepositoryWidget::createToastContainer()
{
    _toastManager = new ToastManager(this);
}

void RepositoryWidget::refreshWidgets(RefreshItems refreshItems)
{
    _refreshItems = refreshItems;
    QTimer::singleShot(0, this, &RepositoryWidget::onRefreshWidgets);
}

void RepositoryWidget::refreshStatusEntries()
{
    StatusOptions statusOptions;
    statusOptions.setExcludeSubmodules(false);
    statusOptions.setShow(StatusShowIndexAndWorkDir);
    GIT::RepositoryStatus status = _repo->status(statusOptions);
    {
        GIT::StatusEntry::List entries;
        entries.appendIfNotPresent(status.modified());
        entries.appendIfNotPresent(status.untracked());
        entries.appendIfNotPresent(status.missing());
        entries.appendIfNotPresent(status.renamedInWorkDir());
        ui->tableUnstagedFiles->createModel(_repo, entries);
    }
    {
        GIT::StatusEntry::List entries;
        entries.appendIfNotPresent(status.staged());
        entries.appendIfNotPresent(status.added());
        entries.appendIfNotPresent(status.renamedInIndex());
        entries.appendIfNotPresent(status.removed());
        ui->tableStagedFiles->createModel(_repo, entries);
    }
}

void RepositoryWidget::updateCommitShaWidget(const GIT::ObjectId& objectId)
{
    // remove old widget
    for(int i = ui->layoutCommitSha->count() - 1;i >= 0;i--) {
        QWidget* widget = ui->layoutCommitSha->itemAt(i)->widget();
        if(dynamic_cast<ShaWidget*>(widget) != nullptr) {
            delete widget;
        }
    }

    // add new sha
    int index = ui->layoutCommitSha->indexOf(ui->textCommitSha);
    if(index > 0) {
        ShaWidget* shaLabel = new ShaWidget(objectId, this);
        ui->layoutCommitSha->insertWidget(index + 1, shaLabel);
    }
}

void RepositoryWidget::updateParentsShaWidget(const GIT::ObjectId& objectId)
{
    ObjectId::List ids;
    ids.append(objectId);
    updateParentsShaWidget(ids);
}

void RepositoryWidget::updateParentsShaWidget(const GIT::ObjectId::List& objectIds)
{
    // remove old widget
    for(int i = ui->layoutParentSha->count() - 1;i >= 0;i--) {
        QWidget* widget = ui->layoutParentSha->itemAt(i)->widget();
        if(dynamic_cast<ParentsShaWidget*>(widget) != nullptr) {
            delete widget;
        }
    }

    // add new sha
    int index = ui->layoutParentSha->indexOf(ui->textParentSha);
    if(index >= 0) {
        ParentsShaWidget* shaWidget = new ParentsShaWidget(objectIds, this);
        connect(shaWidget, &ParentsShaWidget::clicked, this, &RepositoryWidget::onCommitShaClicked);
        ui->layoutParentSha->insertWidget(index + 1, shaWidget);
    }
}

void RepositoryWidget::showLocalBranchCustomContextMenu(const Reference& reference)
{
    if(reference.isNull()) {
        return;
    }

    if(reference.isRemote()) {
        return;
    }

    ui->actionDeleteLocalBranch->setText(QString("Delete %1").arg(reference.friendlyName()));
    ui->actionRenameLocalBranch->setText(QString("Rename %1").arg(reference.friendlyName()));

    ui->actionDeleteLocalBranch->setProperty(ReferenceProperty.toUtf8().constData(), reference.toVariant());
    ui->actionRenameLocalBranch->setProperty(ReferenceProperty.toUtf8().constData(), reference.toVariant());

    QMenu menu(this);
    menu.addAction(ui->actionDeleteLocalBranch);
    menu.addAction(ui->actionRenameLocalBranch);

    menu.exec(QCursor::pos());
}

void RepositoryWidget::showSubmoduleCustomContextMenu(const GIT::Submodule& submodule)
{
    if(submodule.isNull()) {
        return;
    }

    ui->actionInitializeSubmodule->setText(QString("Initialize %1").arg(submodule.name()));
    ui->actionDeleteSubmodule->setText(QString("Delete %1").arg(submodule.name()));

    ui->actionInitializeSubmodule->setProperty(SubmoduleProperty.toUtf8().constData(), submodule.toVariant());
    ui->actionDeleteSubmodule->setProperty(SubmoduleProperty.toUtf8().constData(), submodule.toVariant());

    ui->actionInitializeSubmodule->setEnabled(submodule.isWorkdirInitialized() == false);

    QMenu menu(this);
    menu.addAction(ui->actionInitializeSubmodule);
    menu.addAction(ui->actionDeleteSubmodule);

    menu.exec(QCursor::pos());
}

void RepositoryWidget::showSubmodulesCustomContextMenu()
{
    int uninitialized = _repo->submodules().countWithStatus(Submodule::WorkDirUninitialized);
    ui->actionInitializeAllSubmodules->setText(QString("Initialize %1 submodules").arg(uninitialized));
    ui->actionInitializeAllSubmodules->setEnabled(uninitialized > 0);

    QMenu menu(this);
    menu.addAction(ui->actionAddSubmodule);
    menu.addAction(ui->actionInitializeAllSubmodules);

    menu.exec(QCursor::pos());
}

void RepositoryWidget::resizeEvent(QResizeEvent* event)
{
    ComplexWidget::resizeEvent(event);

    // resize the toast container
    QSize size(ToastWidth, event->size().height() / 2);
    QPoint pos(0, size.height());
    _toastManager->move(pos);
    _toastManager->resize(size);
}

void RepositoryWidget::switchToDiffView()
{
    ui->tabWidget->setCurrentWidget(ui->tabDiffs);
    ui->frameBranches->setVisible(false);
    maybeEnableButtons();
}

void RepositoryWidget::switchToCommitView()
{
    ui->tabWidget->setCurrentWidget(ui->tabCommits);
    ui->frameBranches->setVisible(true);
    maybeEnableButtons();
}

void RepositoryWidget::onRefreshWidgets()
{
    logText(LVL_DEBUG, __FUNCTION__);

    if(_refreshItems & RefreshStatusEntries) {
        refreshStatusEntries();
    }

    if(_refreshItems & RefreshFileSystemTree) {
        ui->treeFileSystem->createModel(_repo);
    }

    if(_refreshItems & RefreshCommitTable) {
        ui->tableCommits->createModel(_repo);
    }

    if(_refreshItems & RefreshLeftSidebar) {
        ui->leftSidebar->createModel(_repo);
    }

    maybeEnableButtons();
}

void RepositoryWidget::createBranch(const QString &branchName)
{
    Branch branch = _repo->createBranch(branchName, true);
    if(branch.isNull()) {
        QMessageBox::warning(this, "Failed to create branch", _repo->errorText());
    }
    refreshWidgets(RefreshAll);
}

void RepositoryWidget::keyPressEvent(QKeyEvent* event)
{
    ComplexWidget::keyPressEvent(event);
    if(event->key() == Qt::Key_Escape && ui->frameBranches->isVisible() == false) {
        switchToCommitView();
    }
}

void RepositoryWidget::maybeEnableButtons()
{
    int stagedFileCount = ui->tableStagedFiles->entries().count();
    int unstagedFileCount = ui->tableUnstagedFiles->entries().count();
    ui->pushCommitChanges->setEnabled(stagedFileCount > 0 && ui->textStageCommitMessage->toPlainText().length() > 0);
    ui->pushStageAll->setEnabled(unstagedFileCount > 0);
    ui->pushUnstageAll->setEnabled(stagedFileCount > 0);
    ui->pushNextDiff->setEnabled(ui->tableDiffs->hasNextDelta());
    ui->pushPreviousDiff->setEnabled(ui->tableDiffs->hasPreviousDelta());
    ui->pushStageDiffFile->setVisible(ui->pushStageDiffFile->property(StageUnstageProperty.toUtf8().constData()) != StageTypeInvalid);

    // Potentially change the text of some widgets
    if(stagedFileCount > 0 && ui->textStageCommitMessage->toPlainText().length() == 0) {
        ui->pushCommitChanges->setText("Enter a commit message");
    }
    else if(stagedFileCount > 0) {
        ui->pushCommitChanges->setText(QString("Commit changes to %1 files").arg(stagedFileCount));
    }
    else {
        ui->pushCommitChanges->setText("Stage some files or changes");
    }
    emit validate();
}

void RepositoryWidget::onRepositoryFileSystemChanged()
{
    if(_filesystemWatchEnabled == false) {
        return;
    }
    logText(LVL_DEBUG, "Repo file system change notification");
    refreshWidgets(RefreshAll);
}

void RepositoryWidget::onFolderClicked(const QString& folderPath)
{
    Q_UNUSED(folderPath)
}

void RepositoryWidget::onIndexEntryClicked(const GIT::IndexEntry& indexEntry)
{
    ui->textPath->setText(indexEntry.path());
    ui->textMode->setText(getModeString(indexEntry.mode()));
    ui->textObjectId->setText(indexEntry.objectId().toString());
    ui->stackedWidget->setCurrentWidget(ui->pageIndexEntry);
}

void RepositoryWidget::onCommitClicked(const GIT::GraphedCommit& commit)
{
    ui->textCommitMessage->setPlainText(commit.message());
    ui->textCommitSignature->setPlainText(commit.author().name());
    ui->textCommitTimestamp->setText(commit.timestamp().toString());
    ui->tableTreeEntries->createModel(_repo, commit.objectId());
    ui->stackedWidget->setCurrentWidget(ui->pageCommitDetails);
    ui->textAuthor->setText(commit.author().name());

    updateCommitShaWidget(commit.objectId());
    updateParentsShaWidget(commit.parentObjectIds());
}

void RepositoryWidget::onStashCommitClicked(const GIT::Stash& stash)
{
    onCommitClicked(stash.workTree());
    updateCommitShaWidget(stash.index().objectId());
    updateParentsShaWidget(stash.base().objectId());
}

void RepositoryWidget::onWorkInProgressClicked()
{
    refreshWidgets(RefreshStatusEntries);
    ui->stackedWidget->setCurrentWidget(ui->pageStageAndCommit);
    maybeEnableButtons();
}

void RepositoryWidget::onReferenceClicked(const GIT::Reference& reference)
{
    ui->tableCommits->selectCommit(reference.objectId());
}

void RepositoryWidget::onLocalReferenceDoubleClicked(const GIT::Reference &reference)
{
    try
    {
        if(_repo->checkoutLocalBranch(reference.friendlyName()) == false) {
            throw CommonException(_repo->errorText());
        }
        refreshWidgets(RefreshAll);

        _toastManager->message(QString("Checked out %1").arg(reference.friendlyName()));
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message());
    }
}

void RepositoryWidget::onRemoteReferenceDoubleClicked(const GIT::Reference& reference)
{
    try
    {
        if(_repo->checkoutRemoteBranch(reference.friendlyName()) == false) {
            throw CommonException(_repo->errorText());
        }
        refreshWidgets(RefreshAll);
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message());
    }
}

void RepositoryWidget::onCommitShaClicked(const GIT::ObjectId& objectId)
{
    ui->tableCommits->selectCommit(objectId);
    switchToCommitView();
}

void RepositoryWidget::onStagedStatusEntryClicked(const GIT::StatusEntry& statusEntry)
{
    // The entry is staged, so we will compare against previous commit
    Tree oldTree = _repo->head().tip().tree();

    CompareOptions compareOptions;
    compareOptions.setSimilarity(SimilarityOptions::renames());
    compareOptions.setContextLines(0);

    DiffDelta::List deltas = _repo->diffTreeToWorkDir(oldTree, QStringList() << statusEntry.path(), true, compareOptions);
    if(deltas.count() == 1) {
        DiffDelta delta = deltas.at(0);
        if(delta.isValid() == false) {
            return;
        }

        ui->tableDiffs->createModelIndexToWorkDir(_repo, delta);
        ui->pushStageDiffFile->setText("Unstage File");
        ui->pushStageDiffFile->setProperty(StageUnstageProperty.toUtf8().constData(), UnstageFile);
        switchToDiffView();
        ui->tableDiffs->scrollToNextDelta();
    }
}

void RepositoryWidget::onUnstagedStatusEntryClicked(const GIT::StatusEntry& statusEntry)
{
    DiffDelta delta = _repo->diffDelta(statusEntry);
    if(delta.isValid() == false) {
        return;
    }

    ui->tableDiffs->createModelIndexToWorkDir(_repo, delta);
    ui->pushStageDiffFile->setText("Stage File");
    ui->pushStageDiffFile->setProperty(StageUnstageProperty.toUtf8().constData(), StageFile);
    switchToDiffView();
    ui->tableDiffs->scrollToNextDelta();
}

void RepositoryWidget::onTreeChangeEntryClicked(const GIT::TreeChangeEntry& treeChangeEntry)
{
    try
    {
        GraphedCommit selectedCommit = ui->tableCommits->currentSelectedCommit();
        if(selectedCommit.isValid() == false) {
            throw CommonException("Selected commit invalid");
        }

        if(selectedCommit.parents().count() != 1) {
            logText(LVL_DEBUG, "More than one parent (checkme)");
            // throw CommonException("Parent invalid (TODO > 1)");
        }

        ObjectId parentId = selectedCommit.parents().at(0).objectId();
        QModelIndex parentIndex = ui->tableCommits->findCommit(parentId);
        if(parentIndex.isValid() == false) {
            throw CommonException("Failed to find parent commit");
        }

        GraphedCommit parentCommit = GraphedCommit::fromVariant(parentIndex.data(CommitRole));
        if(parentCommit.isValid() == false) {
            throw CommonException("Failed to find parent commit (2)");
        }

        CompareOptions compareOptions;
        compareOptions.setSimilarity(SimilarityOptions::renames());
        compareOptions.setContextLines(0);
        DiffDelta::List deltas = _repo->diffCommitToCommit(parentCommit, selectedCommit, compareOptions);
        DiffDelta delta = deltas.findFirstByPath(treeChangeEntry.path());
        if(delta.isValid() == false) {
            throw CommonException("Failed to find delta");
        }

        TreeEntry oldEntry = TreeEntry::findForCommit(parentCommit, delta.oldFile().path());
        TreeEntry newEntry = TreeEntry::findForCommit(selectedCommit, delta.newFile().path());

        ui->tableDiffs->createModelForSingleDelta(_repo, oldEntry, newEntry, delta);
        ui->tableDiffs->scrollToNextDelta();
        ui->pushStageDiffFile->setProperty(StageUnstageProperty.toUtf8().constData(), StageTypeInvalid);
        switchToDiffView();
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message(), QMessageBox::Ok);
    }
}

void RepositoryWidget::onSubmoduleClicked(const GIT::Submodule& submodule)
{
    QString str = submodule.statusDebugString(submodule.status());
    logText(LVL_DEBUG, QString("%1 (%2)").arg(submodule.name()).arg(str));
    logText(LVL_DEBUG, QString("%1 head: %2  idx: %3").arg(submodule.name()).arg(submodule.headCommitId().toString()).arg(submodule.indexCommitId().toString()));
}

void RepositoryWidget::onStagedFilesContextMenuRequested()
{
    QMenu menu(this);
    menu.addAction(ui->actionUnstageFiles);
    menu.addAction(ui->actionDiscardChanges);
    menu.addAction(ui->actionStash);

    // Enable as appropriate
    int selectedCount = ui->tableStagedFiles->selectedEntries().count();
    ui->actionUnstageFiles->setEnabled(selectedCount > 0);
    ui->actionDiscardChanges->setEnabled(selectedCount > 0);
    ui->actionStash->setEnabled(selectedCount > 0);

    // Set menu texts
    if(selectedCount > 1) {
        ui->actionUnstageFiles->setText(QString("Unstage %1 files").arg(selectedCount));
        ui->actionDiscardChanges->setText(QString("Discard %1 files").arg(selectedCount));
        ui->actionStash->setText(QString("Stash %1 files").arg(selectedCount));
    }
    else {
        ui->actionUnstageFiles->setText(QString("Unstage"));
        ui->actionDiscardChanges->setText(QString("Discard"));
        ui->actionStash->setText(QString("Stash"));
    }

    menu.exec(QCursor::pos());
}

void RepositoryWidget::onUnstagedFilesContextMenuRequested()
{
    QMenu menu(this);
    menu.addAction(ui->actionStageFiles);
    menu.addAction(ui->actionDiscardChanges);
    menu.addAction(ui->actionStash);

    // Enable as appropriate
    int selectedCount = ui->tableUnstagedFiles->selectedEntries().count();
    ui->actionStageFiles->setEnabled(selectedCount > 0);
    ui->actionDiscardChanges->setEnabled(selectedCount > 0);
    ui->actionStash->setEnabled(selectedCount > 0);

    // Set menu texts
    if(selectedCount > 1) {
        ui->actionStageFiles->setText(QString("Snstage %1 files").arg(selectedCount));
        ui->actionDiscardChanges->setText(QString("Discard %1 files").arg(selectedCount));
        ui->actionStash->setText(QString("Stash %1 files").arg(selectedCount));
    }
    else {
        ui->actionStageFiles->setText(QString("Stage"));
        ui->actionDiscardChanges->setText(QString("Discard"));
        ui->actionStash->setText(QString("Stash"));
    }

    menu.exec(QCursor::pos());
}

void RepositoryWidget::onCommitTableContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = ui->tableCommits->indexAt(pos);
    if(index.isValid() == false) {
        return;
    }

    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();

    QMenu menu(this);
    menu.addAction(ui->actionApplyStash);
    menu.addAction(ui->actionPopStash);
    menu.addAction(ui->actionDeleteStash);

    // Enable as appropriate
    ui->actionApplyStash->setEnabled(type == GitEntities::Stash);
    ui->actionPopStash->setEnabled(type == GitEntities::Stash);
    ui->actionDeleteStash->setEnabled(type == GitEntities::Stash);

    menu.exec(QCursor::pos());
}

void RepositoryWidget::onLeftSidebarCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->leftSidebar->indexAt(pos);
    if(index.isValid() == false) {
        return;
    }

    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    switch(type) {
    case GitEntities::Reference:
    {
        Reference reference = Reference::fromVariant(index.data(ReferenceRole));
        if(reference.isNull() == false && reference.isLocal()) {
            showLocalBranchCustomContextMenu(reference);
        }
        break;
    }
    case GitEntities::Submodule:
    {
        Submodule submodule = Submodule::fromVariant(index.data(KANOOP::DataRole));
        if(submodule.isNull() == false) {
            showSubmoduleCustomContextMenu(submodule);
        }
        break;
    }
    case GitEntities::TitleItem:
    {
        ControlType controlType = (ControlType)index.data(ControlTypeRole).toInt();
        switch (controlType) {
        case Submodules:
            showSubmodulesCustomContextMenu();
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void RepositoryWidget::onStageFilesClicked()
{
    try
    {
        QStringList paths = ui->tableUnstagedFiles->selectedEntryPaths();
        if(paths.count() == 0) {
            throw CommonException("Nothing to stage");
        }

        if(_repo->stage(paths) == false) {
            throw CommonException("Failed to stage files");
        }
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", QString("%1: %2").arg(e.message()).arg(_repo->errorText()), QMessageBox::Ok);
    }
}

void RepositoryWidget::onUnstageFilesClicked()
{
    try
    {
        QStringList paths = ui->tableStagedFiles->selectedEntryPaths();
        if(paths.count() == 0) {
            throw CommonException("Nothing to unstage");
        }

        if(_repo->unstage(paths) == false) {
            throw CommonException("Failed to unstage files");
        }
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", QString("%1: %2").arg(e.message()).arg(_repo->errorText()), QMessageBox::Ok);
    }
}

void RepositoryWidget::onIgnoreClicked()
{
    UNIMPLEMENTED
}

void RepositoryWidget::stashChanges()
{
    Signature signature = _repo->config()->buildSignature();
    QString message = "WIP";
    if(_repo->stash(signature, message, StashModifierIncludeUntracked) == false) {
        QMessageBox::warning(this, "Error", QString("Failed to stash files: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
}

void RepositoryWidget::onDiscardChangesClicked()
{
    UNIMPLEMENTED
}

void RepositoryWidget::onApplyStashClicked()
{
    UNIMPLEMENTED
}

void RepositoryWidget::onDeleteStashClicked()
{
    Stash stash = ui->tableCommits->currentSelectedStash();
    if(stash.isValid() == false) {
        return;
    }

    if(_repo->deleteStash(stash) == false) {
        QMessageBox::warning(this, "Error", QString("Failed to delete stash: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
}

void RepositoryWidget::popStash()
{
    Stash stash = ui->tableCommits->currentSelectedStash();
    if(stash.isValid() == false) {
        return;
    }

    if(_repo->popStash(stash) == false) {
        QMessageBox::warning(this, "Error", QString("Failed to pop stash: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
}

void RepositoryWidget::showRepoOptionsDialog()
{
    RepoOptionsDialog dlg(_repo, this);
    if(dlg.exec() == QDialog::Accepted) {
        initializeCredentials();
    }
}

void RepositoryWidget::onDeleteLocalBranchClicked()
{
    Reference reference  = Reference::fromVariant(static_cast<QAction*>(sender())->property(ReferenceProperty.toUtf8().constData()));
    if(reference.isNull()) {
        return;
    }

    if(_repo->deleteLocalBranch(reference) == false) {
        QMessageBox::warning(this, "Failed to delete branch", _repo->errorText());
    }
    refreshWidgets(RefreshLeftSidebar);
}

void RepositoryWidget::onRenameLocalBranchClicked()
{
    UNIMPLEMENTED
}

void RepositoryWidget::onInitializeSubmoduleClicked()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action == nullptr) {
        return;
    }

    try
    {
        Submodule submodule = Submodule::fromVariant(action->property(SubmoduleProperty.toUtf8().constData()));
        if(submodule.isNull()) {
            throw CommonException("Invalid submodule (bug)");
        }

        Submodule::SubmoduleStatuses status = submodule.status();
        Q_UNUSED(status)

        if(submodule.initialize() == false) {
            throw CommonException(_repo->errorText());
        }

        ui->leftSidebar->setSubmoduleSpinning(submodule, true);
        ui->leftSidebar->setSubmoduleSpinnerValue(submodule, 0);
        _submoduleUpdateProgressCallback.setSubmodule(submodule);
        if(submodule.update(false, &_credentialResolver, &_submoduleUpdateProgressCallback) == false) {
            throw CommonException(_repo->errorText());
        }
        ui->leftSidebar->setSubmoduleSpinning(submodule, false);
        ui->leftSidebar->setSubmoduleSpinnerValue(submodule, 0);
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message(), QMessageBox::Ok);
    }
}

void RepositoryWidget::onDeleteSubmoduleClicked()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action == nullptr) {
        return;
    }
    try
    {
        Submodule submodule = Submodule::fromVariant(action->property(SubmoduleProperty.toUtf8().constData()));
        if(submodule.isNull()) {
            throw CommonException("Invalid submodule (bug)");
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool result = _repo->deleteSubmodule(submodule);
        QApplication::restoreOverrideCursor();
        if(result == false) {
            throw CommonException(_repo->errorText());
        }
        _toastManager->message(QString("Deleted submodule %1").arg(submodule.name()));
    }
    catch(const CommonException& e)
    {
        _toastManager->errorMessage(e.message());
    }
}

void RepositoryWidget::onInitializeAllSubmodulesClicked()
{
    suspendWatchingFileSystem();

    SubmoduleCloner* cloner = new SubmoduleCloner(_repo);
    connect(cloner, &SubmoduleCloner::progress, this, &RepositoryWidget::onSubmoduleUpdateProgress2);
    connect(cloner, &SubmoduleCloner::submoduleStarted, this, &RepositoryWidget::onSubmoduleUpdateStarted);
    connect(cloner, &SubmoduleCloner::submoduleFinished, this, &RepositoryWidget::onSubmoduleUpdateFinished);
    connect(cloner, &SubmoduleCloner::finished, this, &RepositoryWidget::onSubmoduleUpdaterFinished);
    cloner->start();
}

void RepositoryWidget::onAddSubmoduleClicked()
{
    try
    {
        CloneRepoDialog dlg(this);
        dlg.setLocalPath(_repo->localPath());
        if(dlg.exec() == QDialog::Accepted) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            if(_repo->addSubmodule(dlg.url(), dlg.localPath()) == false) {
                throw CommonException(_repo->errorText());
            }

            refreshWidgets(RefreshLeftSidebar);
            QApplication::restoreOverrideCursor();

            ui->leftSidebar->expandSubmodules();

            Submodule submodule = _repo->submodules().findByUrl(dlg.url());
            _toastManager->message(QString("Successfully added submodule %1").arg(submodule.name()));
        }
    }
    catch(const CommonException& e)
    {
        _toastManager->errorMessage(e.message());
    }
}

void RepositoryWidget::onStageAllChangesClicked()
{
    StatusEntry::List entries = ui->tableUnstagedFiles->entries();
    if(entries.count() > 0 && _repo->stage("*") == false) {
        QMessageBox::warning(this, "Error", QString("Failed to stage files: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
    switchToCommitView();
}

void RepositoryWidget::onStageDiffFileClicked()
{
    if(ui->tabWidget->currentWidget() != ui->tabDiffs) {
        return;
    }

    DiffDelta delta = ui->tableDiffs->delta();
    if(ui->tableUnstagedFiles->entries().findByPath(delta.newFile().path()).isValid() == false) {
        return;
    }

    QStringList filesToStage;
    filesToStage.append(delta.newFile().path());

    if(delta.oldFile().isValid() && delta.oldFile().path() != delta.newFile().path()) {
        filesToStage.append(delta.oldFile().path());
    }

    if(_repo->stage(filesToStage) == false) {
        QMessageBox::warning(this, "Error", QString("Failed to stage file: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }

    ui->tabWidget->setCurrentWidget(ui->tabCommits);
    refreshWidgets(RefreshStatusEntries);
}

void RepositoryWidget::onUnstageAllChangesClicked()
{
    StatusEntry::List entries = ui->tableStagedFiles->entries();
    if(entries.count() > 0 && _repo->unstage("*") == false) {
        QMessageBox::warning(this, "Error", QString("Failed to stage files: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
    switchToCommitView();
}

void RepositoryWidget::onCommitChangesClicked()
{
    Signature signature = _repo->config()->buildSignature();
    QString message = ui->textStageCommitMessage->toPlainText();
    Commit commit = _repo->commit(message, signature, signature);
    if(commit.isValid() == false) {
        QMessageBox::warning(this, "Error", QString("Failed to commit files: %1").arg(_repo->errorText()), QMessageBox::Ok);
    }
    switchToCommitView();
    refreshWidgets(RefreshAll);
}

void RepositoryWidget::pullFromRemote()
{
    try
    {
        Signature signature = _repo->config()->buildSignature();
        MergeResult result = _repo->pull(signature);
        Q_UNUSED(result) // TODO

        _toastManager->message("Successfully pulled from remote");
    }
    catch(const CommonException& e)
    {
        _toastManager->errorMessage(e.message());
        // QMessageBox::warning(this, "Pull Failed", e.message());
    }
    refreshWidgets(RefreshLeftSidebar | RefreshCommitTable);
}

void RepositoryWidget::pushToRemote()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    try
    {
        Branch branch = _repo->currentBranch();
        if(branch.remoteName().isEmpty()) {
            if(_repo->remotes().count() == 0) {
                throw CommonException("No remote to push to");
            }
            if(_repo->remotes().count() > 1) {
                logText(LVL_WARNING, "Multiple remotes not yet supported. Only the first will be used");
            }
            Remote remote = _repo->remotes().at(0);
            QString refspec = branch.canonicalName();
            // QString destinationRefSpec = branch.createRemoteName(remote);
            if(_repo->push(remote, refspec, refspec) == false) {
                throw CommonException(_repo->errorText());
            }
        }
        else if(_repo->push(_repo->currentBranch()) == false) {
            throw CommonException(_repo->errorText());
        }
        _toastManager->message(QString("Successfully pushed %1 to remote").arg(branch.friendlyName()));
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Push Failed", e.message());
    }
    refreshWidgets(RefreshLeftSidebar);
    QApplication::restoreOverrideCursor();
}

void RepositoryWidget::beginCreateBranch()
{
    GraphedCommit commit = _repo->headCommit();
    if(commit.isNull()) {
        return;
    }

    ui->tableCommits->selectCommit(commit.objectId());
    QModelIndex index = ui->tableCommits->findCommit(commit.objectId());
    if(index.isValid() == false) {
        return;
    }

    ui->tableCommits->setEditingBranchName(true);       // puts view in a special mode to allow edit of branch cell
    ui->tableCommits->edit(index);
}

void RepositoryWidget::onNextDiffClicked()
{
    ui->tableDiffs->scrollToNextDelta();
    maybeEnableButtons();
}

void RepositoryWidget::onPreviousDiffClicked()
{
    ui->tableDiffs->scrollToPreviousDelta();
    maybeEnableButtons();
}

void RepositoryWidget::onSubmoduleUpdateProgress(double percent)
{
    SubmoduleUpdateProgressCallback* callback = dynamic_cast<SubmoduleUpdateProgressCallback*>(sender());
    if(callback == nullptr) {
        return;
    }
    Submodule submodule = callback->submodule();
    ui->leftSidebar->setSubmoduleSpinnerValue(submodule, percent);
}

void RepositoryWidget::onSubmoduleUpdateProgress2(const QString& name, double percent)
{
    Submodule submodule = _repo->submodules().findByName(name);
    ui->leftSidebar->setSubmoduleSpinnerValue(submodule, percent);
}

void RepositoryWidget::onSubmoduleUpdateStarted(const QString& name)
{
    Submodule submodule = _repo->submodules().findByName(name);
    ui->leftSidebar->setSubmoduleSpinning(submodule, true);
}

void RepositoryWidget::onSubmoduleUpdateFinished(const QString& name)
{
    Submodule submodule = _repo->submodules().findByName(name);
    ui->leftSidebar->setSubmoduleSpinning(submodule, false);
}

void RepositoryWidget::onSubmoduleUpdaterFinished()
{
    ui->leftSidebar->hideAllSubmoduleSpinners();
    ui->leftSidebar->refreshVisible();
    resumeWatchingFileSystem();

    SubmoduleCloner* cloner = static_cast<SubmoduleCloner*>(sender());
    cloner->deleteLater();
}

void RepositoryWidget::doDebugThing()
{
    static int count = 0;
    if((++count & 0x01) != 0) {
        _toastManager->message(QString("%1 Succesfuly pushed some stuff from my desktop to somewhere else on the screen").arg(count));
    }
    else {
        _toastManager->errorMessage(QString("%1 Failed to pushed some stuff from my desktop to somewhere else on the screen").arg(count));
    }

#if 0
    GraphedCommit::List commits = _repo->commitGraph();
    QStringList headers = {
        "Message",
        "Branch",
        "SHA",
        "Merge Base",
        "Merge From",
        "Merged Into",
        "Level",
        "Max Level",
        "Merge",
        "Stash",
        "Head",
    };

    // make output object
    ColumnatedOutput output(headers);

    for(const GraphedCommit& commit : commits) {
        QVector<QVariant> values;
        values.append(commit.message().trimmed().left(64));
        values.append(commit.friendlyBranchName());
        values.append(commit.objectId().toString(6));
        values.append(commit.mergeBase().isValid() ? commit.mergeBase().toString(6) : "");
        values.append(commit.mergeFrom().isValid() ? commit.mergeFrom().toString(6) : "");
        values.append(commit.mergedInto().isValid() ? commit.mergedInto().toString(6) : "");
        values.append(commit.level());
        values.append(commit.maxLevel());
        values.append(StringUtil::toString(commit.isMerge()));
        values.append(StringUtil::toString(commit.isStash()));
        values.append(StringUtil::toString(commit.isHead()));
        for(int i = 0;i < values.count();i++)
            output.setValue(i, values[i]);
        output.nextRow();
    }

    QString result = output.toString();
    QTextStream(stdout) << result << Qt::endl;
#endif
}

void RepositoryWidget::drawDebugArc()
{
    int startAngle = ui->textStartAngle->text().toInt() * 16;
    int spanAngle = ui->textSpanAngle->text().toInt() * 16;

    int LevelWidth = 32;
    int RowHeight = 60;
    int Levels = 7;

    QPixmap pixmap(ui->labelGraphic->size());
    pixmap.fill();

    GraphItemType type;
    if(ui->radioDownToLeft->isChecked()) {
        type = DownToLeft;
    }
    if(ui->radioDownToRight->isChecked()) {
        type = DownToRight;
    }
    if(ui->radioUpToLeft->isChecked()) {
        type = UpToLeft;
    }
    if(ui->radioUpToRight->isChecked()) {
        type = UpToRight;
    }
    if(ui->radioRightThenUp->isChecked()) {
        type = RightThenUp;
    }
    if(ui->radioRightThenDown->isChecked()) {
        type = RightThenDown;
    }
    if(ui->radioLeftThenDown->isChecked()) {
        type = LeftThenDown;
    }
    if(ui->radioLeftThenUp->isChecked()) {
        type = LeftThenUp;
    }

    QPainter painter(&pixmap);

    int RowCount = 8;
    for(int row = 0;row < RowCount;row++) {
        int rowY = row * RowHeight;
        painter.setPen(Colors::yellowgreen);
        Rectangle rowRect(0, rowY, Levels * LevelWidth, RowHeight);
        painter.drawRect(rowRect);
        painter.setPen(Colors::lightblue);
        for(int level = 0;level < Levels;level++) {
            int x = level * LevelWidth;
            painter.drawLine(Point(x, rowY), Point(x, rowY + RowHeight));
        }
    }


    int row = ui->textRow->text().toInt();
    int level = ui->textLevel->text().toInt();

    Rectangle levelRect(level * LevelWidth, row * RowHeight, LevelWidth, RowHeight);
    painter.setPen(Colors::black);
    painter.drawRect(levelRect);

    Point dotPos = levelRect.centerPoint();
    Size rectSize(LevelWidth * 2, RowHeight);

    Rectangle arcRect;
    switch(type) {
    case DownToLeft:
        arcRect = Rectangle(Point(levelRect.x() - (LevelWidth / 2), dotPos.y()), rectSize);
        startAngle = -(270 * 16);
        spanAngle = (90 * 16);
        break;
    case DownToRight:
        arcRect = Rectangle(Point(levelRect.x() - (LevelWidth / 2), dotPos.y()), rectSize);
        startAngle = -(270 * 16);
        spanAngle = -(90 * 16);
        break;
    case UpToLeft:
        arcRect = Rectangle(Point(levelRect.x() - (LevelWidth / 2), dotPos.y() - RowHeight), rectSize);
        startAngle = (270 * 16);
        spanAngle = -(90 * 16);
        break;
    case UpToRight:
        arcRect = Rectangle(Point(levelRect.x() - (LevelWidth / 2), dotPos.y() - RowHeight), rectSize);
        startAngle = (270 * 16);
        spanAngle = (90 * 16);
        break;
    default:
        break;
    }

    painter.setPen(Colors::red);
    painter.drawRect(arcRect);

    painter.setPen(Colors::blue);
    painter.drawArc(arcRect, startAngle, spanAngle);

    QList<GraphItemType> types = {
        UpToLeft, UpToRight, DownToLeft, DownToRight,
        LeftThenDown, LeftThenUp, RightThenDown, RightThenUp
    };

    int x = (LevelWidth * 3) - (LevelWidth / 2); //(ui->labelGraphic->width() - LevelWidth) - 1;
    int y = RowHeight / 2;
    for(GraphItemType type : types) {
        QPixmap arc = GitCommitGraphStyledItemDelegate::createArc(LevelWidth, RowHeight, type, GitGraphPalette());

        int drawX = 0;
        int drawY = 0;
        switch (type) {
        case UpToLeft:
        case LeftThenUp:
            drawX = x - LevelWidth;
            drawY = y - (RowHeight / 2);
            break;
        case UpToRight:
        case RightThenUp:
            drawX = x;
            drawY = y - (RowHeight / 2);
            break;
        case DownToLeft:
        case LeftThenDown:
            drawX = x - LevelWidth;
            drawY = y;
            break;
        case DownToRight:
        case RightThenDown:
            drawX = x;
            drawY = y;
            break;
        default:
            break;
        }

        logText(LVL_DEBUG, QString("Draw at %1, %2").arg(drawX).arg(drawY));
        painter.drawPixmap(drawX, drawY, arc);
        y += RowHeight;
    }

    ui->labelGraphic->setPixmap(pixmap);
}
