#include "statusentrytablemodel.h"
#include "kanoopgittypes.h"
#include <QFileInfo>

#include <Kanoop/pathutil.h>

#include <QFont>
#include <gitassets.h>

#include <Kanoop/gui/resources.h>

using namespace GIT;

StatusEntryTableModel::StatusEntryTableModel(Repository* repo, const StatusEntry::List& statusEntries, QObject *parent) :
    AbstractTableModel("committable", parent),
    _repo(repo)
{
    appendColumnHeader(CH_File, "File");

    for(const StatusEntry& statusEntry : statusEntries) {
        appendRootItem(new StatusEntryItem(statusEntry, this));
    }
}



QVariant StatusEntryTableModel::StatusEntryItem::data(const QModelIndex &index, int role) const
{
    QVariant result;

    TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
    if(header.type() == CH_File) {
        switch(role) {
        case Qt::DisplayRole:
            result = _statusEntry.path();
            break;
        case Qt::DecorationRole:
        {
            FileStatuses status = _statusEntry.status();
            if(status.testFlag(ModifiedInIndex) || status.testFlag(ModifiedInWorkdir)) {
                result = Resources::getIcon(GitAssets::Pencil);
            }
            else if(status.testFlag(DeletedFromIndex) || status.testFlag(DeletedFromWorkdir)) {
                result = Resources::getIcon(GitAssets::MinusRed);
            }
            else if(status.testFlag(NewInIndex) || status.testFlag(NewInWorkdir)) {
                result = Resources::getIcon(GitAssets::PlusGreen);
            }
            else if(status.testFlag(RenamedInWorkdir) || status.testFlag(RenamedInIndex)) {
                result = Resources::getIcon(GitAssets::RenameFile);
            }
            break;
        }
        case StatusEntryRole:
            result = _statusEntry.toVariant();
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

