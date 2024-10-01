#include "leftsidebartreeview.h"
#include "kanoopgittypes.h"
#include "leftsidebartreemodel.h"

#include <repoconfig.h>
#include <settings.h>

using namespace GIT;

LeftSidebarTreeView::LeftSidebarTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void LeftSidebarTreeView::createModel(GIT::Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

    _repo = repo;

    LeftSidebarTreeModel* tableModel = new LeftSidebarTreeModel(repo, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &LeftSidebarTreeView::onCurrentIndexChanged);
    connect(this, &LeftSidebarTreeView::expanded, this, &LeftSidebarTreeView::onExpanded);
    connect(this, &LeftSidebarTreeView::collapsed, this, &LeftSidebarTreeView::onCollapsed);

    RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
    if(config.localBranchesVisible()) {
        expandRecursively(tableModel->localBranchesIndex());
    }

    if(config.remoteBranchesVisible()) {
        expandRecursively(tableModel->remoteBranchesIndex());
    }

    if(config.submodulesVisible()) {
        expandRecursively(tableModel->submodulesIndex());
    }
}

void LeftSidebarTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        GitEntities::Type type = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Folder) {
            QString path = current.data(RelativePathRole).toString();
            emit folderClicked(path);
        }
        else if(type == GitEntities::Reference) {
            Reference reference = Reference::fromVariant(current.data(ReferenceRole));
            if(reference.isNull() == false) {
                emit referenceClicked(reference);
            }
        }
    }
}

void LeftSidebarTreeView::onDoubleClicked(const QModelIndex& index)
{
    if(index.isValid()) {
        GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Reference) {
            Reference reference = Reference::fromVariant(index.data(ReferenceRole));
            logText(LVL_DEBUG, QString("DblClick: %1").arg(reference.canonicalName()));
            if(reference.isNull() == false) {
                emit referenceDoubleClicked(reference);
            }
        }
    }
}

void LeftSidebarTreeView::onExpanded(const QModelIndex& index)
{
    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    if(type == GitEntities::TitleItem) {
        RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
        ControlType controlType = (ControlType)index.data(ControlTypeRole).toInt();
        switch(controlType) {
        case LocalBranches:
            config.setLocalBranchesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        case RemoteBranches:
            config.setRemoteBranchesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        case Submodules:
            config.setSubmodulesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        default:
            break;
        }
    }
}

void LeftSidebarTreeView::onCollapsed(const QModelIndex& index)
{
    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    if(type == GitEntities::TitleItem) {
        RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
        ControlType controlType = (ControlType)index.data(ControlTypeRole).toInt();
        switch(controlType) {
        case LocalBranches:
            config.setLocalBranchesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        case RemoteBranches:
            config.setRemoteBranchesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        case Submodules:
            config.setSubmodulesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        default:
            break;
        }
    }
}
