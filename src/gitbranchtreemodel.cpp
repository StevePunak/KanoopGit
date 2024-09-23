#include "gitbranchtreemodel.h"
#include "gitentities.h"
#include "gitroles.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QFont>

#include <Kanoop/pathutil.h>

using namespace GIT;

GitBranchTreeModel::GitBranchTreeModel(Repository* repo, BranchType typeToShow, QObject *parent) :
    AbstractTreeModel("branchtree", parent),
    _repo(repo), _typeToShow(typeToShow)
{
    appendColumnHeader(ColHeaders::CH_Name, "Name");
    switch(_typeToShow) {
    case LocalBranch:
        loadBranches(repo->references().localBranchReferences());
        break;
    case RemoteBranch:
        for(const Remote& remote : _repo->remotes()) {
            Reference::List references = _repo->remoteReferences(remote.name());
            loadBranches(references);
        }
        break;
    default:
        break;
    }
}

void GitBranchTreeModel::refresh()
{
    QModelIndex topLeft = index(0, 0, QModelIndex());
    int rows = rowCount(QModelIndex());
    int cols = columnCount(QModelIndex());
    QModelIndex bottomRight = index(rows - 1, cols - 1, QModelIndex());
    emit dataChanged(topLeft, bottomRight);
}

void GitBranchTreeModel::loadBranches(const Reference::List& references)
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
                ensureFolder(path);
            }
            QModelIndex parentIndex = findFolderIndex(parentPath(name));
            if(parentIndex.isValid()) {
                TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
                parentItem->appendChild(new ReferenceItem(reference, this));
            }
            else {
                logText(LVL_ERROR, "Parent path not exist");
            }
        }
        else {
            QModelIndex parentIndex = findRemoteIndex(parentPath(name));
            if(parentIndex.isValid()) {
                TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
                parentItem->appendChild(new ReferenceItem(reference, this));
            }
            else {
                appendRootItem(new ReferenceItem(reference, this));
            }
        }
    }
}

void GitBranchTreeModel::ensureFolder(const QString& path)
{
    QModelIndex index = findFolderIndex(path);
    if(index.isValid() == false) {
        QModelIndex parentIndex = findFolderIndex(parentPath(path));
        if(parentIndex.isValid()) {
            TreeBaseItem* parentItem = static_cast<TreeBaseItem*>(parentIndex.internalPointer());
            parentItem->appendChild(new FolderItem(path, this));
        }
        else {
            appendRootItem(new FolderItem(path, this));
        }
    }
}

QModelIndex GitBranchTreeModel::findBranchIndex(const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, AbsolutePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex GitBranchTreeModel::findReferenceIndex(const GIT::ObjectId& objectId) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, ObjectIdRole, objectId.toVariant(), 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex GitBranchTreeModel::findFolderIndex(const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, RelativePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex GitBranchTreeModel::findRemoteIndex(const QString& remoteName) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, RemoteNameRole, remoteName, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QString GitBranchTreeModel::parentPath(const QString& path)
{
    return PathUtil::popLevel(path);
}


// -------------------------------- FolderItem --------------------------------


GitBranchTreeModel::FolderItem::FolderItem(const QString& path, GitBranchTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Folder), model),
    _path(path)
{
    QFileInfo fileInfo(path);
    _folderName = fileInfo.fileName();
}

QVariant GitBranchTreeModel::FolderItem::data(const QModelIndex& index, int role) const
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


GitBranchTreeModel::ReferenceItem::ReferenceItem(const Reference& reference, GitBranchTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Reference), model),
    _reference(reference)
{
    QStringList parts = reference.name().split('/');
    _text = parts.count() > 0 ? parts.last() : reference.name();
}

QVariant GitBranchTreeModel::ReferenceItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _text;
            break;
        case Qt::FontRole:
        {
            GitBranchTreeModel* model = static_cast<GitBranchTreeModel*>(AbstractModelItem::model());
            Repository* repo = model->_repo;
            if(model->_typeToShow == LocalBranch && _reference.objectId() == repo->head().reference().objectId()) {
                QFont font;
                font.setBold(true);
                result = font;
            }
            break;
        }
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

