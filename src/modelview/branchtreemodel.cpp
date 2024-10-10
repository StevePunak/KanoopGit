#include "branchtreemodel.h"
#include "gitentities.h"
#include "kanoopgittypes.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QFont>

#include <Kanoop/pathutil.h>

using namespace GIT;

BranchTreeModel::BranchTreeModel(Repository* repo, BranchType typeToShow, QObject *parent) :
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

void BranchTreeModel::refresh()
{
    QModelIndex topLeft = index(0, 0, QModelIndex());
    int rows = rowCount(QModelIndex());
    int cols = columnCount(QModelIndex());
    QModelIndex bottomRight = index(rows - 1, cols - 1, QModelIndex());
    emit dataChanged(topLeft, bottomRight);
}

void BranchTreeModel::loadBranches(const Reference::List& references)
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

void BranchTreeModel::ensureFolder(const QString& path)
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

QModelIndex BranchTreeModel::findBranchIndex(const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, AbsolutePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex BranchTreeModel::findReferenceIndex(const GIT::ObjectId& objectId) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, ObjectIdRole, objectId.toVariant(), 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex BranchTreeModel::findFolderIndex(const QString& path) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, RelativePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QModelIndex BranchTreeModel::findRemoteIndex(const QString& remoteName) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, RemoteNameRole, remoteName, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QString BranchTreeModel::parentPath(const QString& path)
{
    return PathUtil::popLevel(path);
}


// -------------------------------- FolderItem --------------------------------


BranchTreeModel::FolderItem::FolderItem(const QString& path, BranchTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Folder), model),
    _path(path)
{
    QFileInfo fileInfo(path);
    _folderName = fileInfo.fileName();
}

QVariant BranchTreeModel::FolderItem::data(const QModelIndex& index, int role) const
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


BranchTreeModel::ReferenceItem::ReferenceItem(const Reference& reference, BranchTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Reference), model),
    _reference(reference), _isCurrentBranch(false)
{
    QStringList parts = reference.name().split('/');
    _text = parts.count() > 0 ? parts.last() : reference.name();
    Branch currentBranch = model->_repo->currentBranch();
    _isCurrentBranch = model->_typeToShow == LocalBranch && reference.friendlyName() == currentBranch.friendlyName();
}

QVariant BranchTreeModel::ReferenceItem::data(const QModelIndex& index, int role) const
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

