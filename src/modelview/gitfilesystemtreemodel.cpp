#include "gitassets.h"
#include "gitentities.h"
#include "gitfilesystemtreemodel.h"
#include "gitroles.h"

#include <QDirIterator>
#include <QFileInfo>

#include <Kanoop/pathutil.h>

#include <Kanoop/gui/resources.h>

using namespace GIT;

GitFileSystemTreeModel::GitFileSystemTreeModel(Repository* repo, QObject *parent) :
    AbstractTreeModel("gitfstree", parent),
    _repo(repo)
{
    appendColumnHeader(CH_Name, "Name");
    loadDirectories();
    loadFiles();
    // for(const IndexEntry& indexEntry : _repo->index()->entries()) {
    //     createIfNotExists(indexEntry);
    // }
}

void GitFileSystemTreeModel::refresh()
{
    QModelIndex topLeft = index(0, 0, QModelIndex());
    int rows = rowCount(QModelIndex());
    int cols = columnCount(QModelIndex());
    QModelIndex bottomRight = index(rows - 1, cols - 1, QModelIndex());
    emit dataChanged(topLeft, bottomRight);
}

void GitFileSystemTreeModel::loadDirectories()
{
    QDirIterator it(_repo->localPath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        QDir dir(it.next());
        if(dir.exists()) {
            QModelIndex parentIndex = findFolderIndex(parentPath(dir.absolutePath()));
            if(parentIndex.isValid()) {
                FolderItem* parentItem = static_cast<FolderItem*>(parentIndex.internalPointer());
                parentItem->appendChild(new FolderItem(dir.absolutePath(), this));
            }
            else {
                appendRootItem(new FolderItem(dir.absolutePath(), this));
            }
        }
    }
}

void GitFileSystemTreeModel::loadFiles()
{
    QDirIterator it(_repo->localPath(), QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        QFileInfo fileinfo(it.next());
        QString parentDir = fileinfo.absolutePath();
        QModelIndex parentIndex = findFolderIndex(parentDir);
        if(parentIndex.isValid()) {
            FolderItem* parentItem = static_cast<FolderItem*>(parentIndex.internalPointer());
            parentItem->appendChild(new FileItem(fileinfo.absoluteFilePath(), this));
        }
        else {
            appendRootItem(new FileItem(fileinfo.absoluteFilePath(), this));
        }
    }
}

QModelIndex GitFileSystemTreeModel::findFolderIndex(const QString& path)
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, AbsolutePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QString GitFileSystemTreeModel::parentPath(const QString& path)
{
    return PathUtil::popLevel(path);
}

// -------------------------------- TreeBaseItem --------------------------------


GitFileSystemTreeModel::TreeBaseItem::TreeBaseItem(const EntityMetadata &metadata, const QString& path, GitFileSystemTreeModel* model) :
    AbstractModelItem(metadata, model),
    _absolutePath(path)
{
    QFileInfo fileInfo(_absolutePath);
    _filename = fileInfo.fileName();
    _relativePath = _absolutePath.mid(repo()->localPath().length() + 1);
}

QVariant GitFileSystemTreeModel::TreeBaseItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _filename;
            break;
        case AbsolutePathRole:
            result = _absolutePath;
            break;
        case RelativePathRole:
            result = _relativePath;
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = AbstractModelItem::data(index, role);
    }
    return result;
}

// -------------------------------- FolderItem --------------------------------


GitFileSystemTreeModel::FolderItem::FolderItem(const QString& path, GitFileSystemTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Folder), path, model)
{
    QFileInfo fileInfo(path);
    _folderName = fileInfo.fileName();
    _icon = Resources::getIcon(GitAssets::Folder);
}

QVariant GitFileSystemTreeModel::FolderItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DecorationRole:
            return _icon;
            break;
        case AbsolutePathRole:
            result = absolutePath();
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


// -------------------------------- FileItem --------------------------------


GitFileSystemTreeModel::FileItem::FileItem(const QString& path, GitFileSystemTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::File), path, model) {}

QVariant GitFileSystemTreeModel::FileItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DecorationRole:
        {
            StatusEntry entry = repo()->status().entries().findByPath(relativePath());
            if(entry.isValid()) {
                switch(entry.status()) {
                case NewInIndex:
                case NewInWorkdir:
                    result = Resources::getIcon(GitAssets::PlusGreen);
                    break;
                case ModifiedInWorkdir:
                    result = Resources::getIcon(GitAssets::LedRed);
                    break;
                case ModifiedInIndex:
                    result = Resources::getIcon(GitAssets::PlusWhiteOnGrey);
                    break;
                default:
                    Log::logText(LVL_DEBUG, QString("Unhandled status %1 for %2").arg(getFileStatusString(entry.status())).arg(entry.path()));
                    break;
                }
            }
            else {
                IndexEntry indexEntry = repo()->index()->findByPath(relativePath());
                if(indexEntry.isValid()) {
                    result = Resources::getIcon(GitAssets::CheckGreen);
                }
                else {
                    // Ignored
                    result = Resources::getIcon(GitAssets::DashWhiteOnGrey);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}

