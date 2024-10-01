#include "leftsidebartreemodel.h"
#include <QFileInfo>
#include <QStandardPaths>

#include "kanoopgittypes.h"
#include <Kanoop/gui/resources.h>
#include <Kanoop/pathutil.h>
#include <QFont>
#include <gitassets.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

LeftSidebarTreeModel::LeftSidebarTreeModel(GIT::Repository* repo, QObject *parent) :
    AbstractTreeModel("leftbar", parent),
    _repo(repo)
{
    // appendColumnHeader(CH_RecentRepos, "Recent Repos");

    createLocalBranchesLeaf();
    createRemoteBranchesLeaf();
    createSubmodulesLeaf();
}

QModelIndex LeftSidebarTreeModel::localBranchesIndex() const
{
    QModelIndex result;
    QModelIndexList found = match(index(0, 0, QModelIndex()), ControlTypeRole, LocalBranches, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex LeftSidebarTreeModel::remoteBranchesIndex() const
{
    QModelIndex result;
    QModelIndexList found = match(index(0, 0, QModelIndex()), ControlTypeRole, RemoteBranches, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

void LeftSidebarTreeModel::createLocalBranchesLeaf()
{
    TitleItem* titleItem = static_cast<TitleItem*>(appendRootItem(new TitleItem("Local Branches", Resources::getIcon(GitAssets::Computer), LocalBranches, this)));
    loadBranches(titleItem, _repo->references().localBranchReferences(), true);
}

void LeftSidebarTreeModel::createRemoteBranchesLeaf()
{
    TitleItem* titleItem = static_cast<TitleItem*>(appendRootItem(new TitleItem("Remote Branches", Resources::getIcon(GitAssets::Cloud), RemoteBranches, this)));
    for(const Remote& remote : _repo->remotes()) {
        Reference::List references = _repo->remoteReferences(remote.name());
        loadBranches(titleItem, references, false);
    }
}

void LeftSidebarTreeModel::createSubmodulesLeaf()
{
    TitleItem* titleItem = static_cast<TitleItem*>(appendRootItem(new TitleItem("Submodules", Resources::getIcon(GitAssets::Submodules), Submodules, this)));
    for(const Submodule& submodule : _repo->submodules()) {
        titleItem->appendChild(new SubmoduleItem(submodule, this));
    }
}

void LeftSidebarTreeModel::loadBranches(AbstractModelItem* rootItem, const GIT::Reference::List& references, bool local)
{
    for(const Reference& reference : references) {
        if(reference.type() != DirectReferenceType) {
            continue;
        }

        QString name = reference.friendlyName();
        QStringList parts = name.split('/');
        if(parts.count() > 1) {
            for(int i = 0;i < parts.count() - 1;i++) {
                QStringList pathParts = parts.mid(0, i + 1);
                QString path = PathUtil::combine(pathParts);
                ensureFolder(rootItem, path);
            }
            QModelIndex parentIndex = findFolderIndex(rootItem, parentPath(name));
            if(parentIndex.isValid()) {
                TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
                if(local) {
                    parentItem->appendChild(new LocalBranchItem(reference, this));
                }
                else {
                    parentItem->appendChild(new RemoteBranchItem(reference, this));
                }
            }
            else {
                logText(LVL_ERROR, "Parent path not exist (bug)");
            }
        }
        else {
            QModelIndex parentIndex = findRemoteIndex(rootItem, parentPath(name));
            if(parentIndex.isValid()) {
                TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
                if(local) {
                    parentItem->appendChild(new LocalBranchItem(reference, this));
                }
                else {
                    parentItem->appendChild(new RemoteBranchItem(reference, this));
                }
            }
            else {
                if(local) {
                    rootItem->appendChild(new LocalBranchItem(reference, this));
                }
                else {
                    rootItem->appendChild(new RemoteBranchItem(reference, this));
                }
            }
        }
    }
}

void LeftSidebarTreeModel::ensureFolder(AbstractModelItem* rootItem, const QString& path)
{
    QModelIndex index = findFolderIndex(rootItem, path);
    if(index.isValid() == false) {
        QModelIndex parentIndex = findFolderIndex(rootItem, parentPath(path));
        if(parentIndex.isValid()) {
            TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
            parentItem->appendChild(new FolderItem(path, this));
        }
        else {
            rootItem->appendChild(new FolderItem(path, this));
        }
    }
}

QModelIndex LeftSidebarTreeModel::findBranchIndex(AbstractModelItem* rootItem, const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = findTitleItemIndex(rootItem);
    QModelIndexList found = match(startSearchIndex, AbsolutePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex LeftSidebarTreeModel::findReferenceIndex(AbstractModelItem* rootItem, const GIT::ObjectId& objectId) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = findTitleItemIndex(rootItem);
    QModelIndexList found = match(startSearchIndex, ObjectIdRole, objectId.toVariant(), 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex LeftSidebarTreeModel::findFolderIndex(AbstractModelItem* rootItem, const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = findTitleItemIndex(rootItem);
    QModelIndexList found = match(startSearchIndex, RelativePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex LeftSidebarTreeModel::findRemoteIndex(AbstractModelItem* rootItem, const QString& remoteName) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = findTitleItemIndex(rootItem);
    QModelIndexList found = match(startSearchIndex, RemoteNameRole, remoteName, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex LeftSidebarTreeModel::findTitleItemIndex(AbstractModelItem* item) const
{
    QModelIndex result;
    int row = rootItems().indexOf(item);
    if(row >= 0) {
        result = index(row, 0);
    }
    return result;
}

QString LeftSidebarTreeModel::parentPath(const QString& path)
{
    return PathUtil::popLevel(path);
}

// -------------------------------- TitleItem --------------------------------

LeftSidebarTreeModel::TitleItem::TitleItem(const QString& text, const QIcon& icon, ControlType controlType, LeftSidebarTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::TitleItem), model),
    _title(text), _icon(icon), _controlType(controlType)
{
    _font.setPointSize(13);
    _font.setBold(true);
}

QVariant LeftSidebarTreeModel::TitleItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _title;
            break;
        case Qt::DecorationRole:
            if(_icon.isNull() == false) {
                result = _icon;
            }
            break;
        case Qt::ForegroundRole:
            result = Colors::darkblue;
            break;
        case Qt::FontRole:
            result = _font;
            break;
        case ControlTypeRole:
            result = _controlType;
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}

// -------------------------------- SubmoduleItem --------------------------------

QVariant LeftSidebarTreeModel::SubmoduleItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _submodule.name();
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;}

// -------------------------------- FolderItem --------------------------------

LeftSidebarTreeModel::FolderItem::FolderItem(const QString& path, LeftSidebarTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Folder), model),
    _path(path)
{
    QFileInfo fileInfo(path);
    _folderName = fileInfo.fileName();
}

QVariant LeftSidebarTreeModel::FolderItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _folderName;
            break;
        case RelativePathRole:
            result = _path;
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}

// -------------------------------- ReferenceItem --------------------------------

LeftSidebarTreeModel::ReferenceItem::ReferenceItem(const GIT::Reference& reference, LeftSidebarTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Reference), model),
    _reference(reference), _isCurrentBranch(false)
{
    QStringList parts = reference.name().split('/');
    _text = parts.count() > 0 ? parts.last() : reference.name();
    Branch currentBranch = model->_repo->currentBranch();
    _isCurrentBranch = reference.friendlyName() == currentBranch.friendlyName();
}

QVariant LeftSidebarTreeModel::ReferenceItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _text;
            break;
        case Qt::FontRole:
            if(_isCurrentBranch) {
                QFont font;
                font.setBold(true);
                result = font;
            }
            break;
        case ReferenceRole:
            result = _reference.toVariant();
            break;
        case ObjectIdRole:
            result = _reference.objectId().toVariant();
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}


