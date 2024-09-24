#include "gittreeentrytablemodel.h"

#include <QFileInfo>

#include <Kanoop/pathutil.h>
#include "gitassets.h"
#include "gitroles.h"

#include <Kanoop/gui/resources.h>

using namespace GIT;

GitTreeEntryTableModel::GitTreeEntryTableModel(Repository* repo, const ObjectId& commitId, QObject *parent) :
    AbstractTableModel("committable", parent),
    _repo(repo)
{
    appendColumnHeader(CH_File, "File");
    Commit thisCommit = _repo->findCommit(commitId);
    if(thisCommit.isValid()) {
        Commit::List parents = thisCommit.parents();
        if(parents.count() == 1) {
            Commit parentCommit = _repo->findCommit(thisCommit.parents().at(0).objectId());
            if(parentCommit.isValid()) {
                Tree thisTree = thisCommit.tree();
                Tree parentTree = parentCommit.tree();
                TreeChanges changes = repo->diff()->compare(parentTree, thisTree);
                for(const TreeChangeEntry& entry : changes) {
                    appendRootItem(new TreeChangeEntryItem(entry, this));
                }
            }

        }
        else if(parents.count() > 1) {
            Tree thisTree = thisCommit.tree();
            for(const Commit& parentCommit : parents) {
                Tree parentTree = parentCommit.tree();
                TreeChanges changes = repo->diff()->compare(parentTree, thisTree);
                for(const TreeChangeEntry& entry : changes) {
                    appendRootItem(new TreeChangeEntryItem(entry, this));
                }
            }
        }
    }
}

QVariant GitTreeEntryTableModel::TreeChangeEntryItem::data(const QModelIndex &index, int role) const
{
    QVariant result;
    switch(role) {
    case Qt::DisplayRole:
    {
        TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
        switch(header.type()) {
        case CH_File:
            result = _treeChangeEntry.path();
            break;
        default:
            break;
        }
    }
    break;
    case Qt::DecorationRole:
    {
        TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
        switch(header.type()) {
        case CH_File:
        {
            switch(_treeChangeEntry.changeKind()) {
            case ChangeKindAdded:
                result = Resources::getIcon(GitAssets::PlusGreen);
                break;
            case ChangeKindDeleted:
                result = Resources::getIcon(GitAssets::MinusRed);
                break;
            case ChangeKindModified:
                result = Resources::getIcon(GitAssets::Pencil);
                break;
            case ChangeKindRenamed:
                result = Resources::getIcon(GitAssets::DashBlueOnWhite);
                break;
            case ChangeKindConflicted:
                result = Resources::getIcon(GitAssets::WarningRed);
                break;
            case ChangeKindCopied:
            case ChangeKindIgnored:
            case ChangeKindUntracked:
            case ChangeKindTypeChanged:
            case ChangeKindUnreadable:
            case ChangeKindUnmodified:
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case TreeChangeEntryRole:
        result = _treeChangeEntry.toVariant();
        break;
    default:
        break;
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}
