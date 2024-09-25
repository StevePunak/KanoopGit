#include "statusentrytableview.h"
#include "statusentrytablemodel.h"
#include "gitroles.h"

#include <QHeaderView>
#include <QPainter>
#include <QItemSelectionModel>
#include <git2qt.h>

#include <Kanoop/geometry/rectangle.h>

#include <Kanoop/commonexception.h>

using namespace GIT;

StatusEntryTableView::StatusEntryTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void StatusEntryTableView::createModel(Repository* repo, const StatusEntry::List& entries)
{
    _repo = repo;

    if(model() != nullptr) {
        delete model();
    }

    StatusEntryTableModel* tableModel = new StatusEntryTableModel(repo, entries, this);
    setSourceModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &StatusEntryTableView::onCurrentSelectionChanged);

    restoreHeaderStates();
}

StatusEntry::List StatusEntryTableView::entries() const
{
    StatusEntry::List result;
    if(sourceModel() == nullptr) {
        return result;
    }

    for(int i = 0;i < sourceModel()->rowCount();i++) {
        QModelIndex index = sourceModel()->index(i, 0);
        StatusEntry entry = StatusEntry::fromVariant(index.data(StatusEntryRole));
        if(entry.isValid()) {
            result.append(entry);
        }
    }
    return result;
}

StatusEntry::List StatusEntryTableView::selectedEntries() const
{
    StatusEntry::List result;

    if(sourceModel() == nullptr) {
        return result;
    }

    QModelIndexList indexes = selectionModel()->selectedRows();
    for(const QModelIndex& index : indexes) {
        StatusEntry entry = StatusEntry::fromVariant(index.data(StatusEntryRole));
        if(entry.isValid()) {
            result.append(entry);
        }
    }
    return result;
}

QStringList StatusEntryTableView::selectedEntryPaths() const
{
    QStringList paths;
    StatusEntry::List entries = selectedEntries();
    try
    {
        for(const StatusEntry& entry : entries) {
            if(entry.status() == RenamedInWorkdir) {
                if(entry.indexToWorkDirRenameDetails().newFilePath().isEmpty()) {
                    throw CommonException("Failed to find rename new file path");
                }
                if(entry.indexToWorkDirRenameDetails().oldFilePath().isEmpty()) {
                    throw CommonException("Failed to find rename old file path");
                }
                paths.append(entry.indexToWorkDirRenameDetails().newFilePath());
                paths.append(entry.indexToWorkDirRenameDetails().oldFilePath());
            }
            else if(entry.status() == RenamedInIndex) {
                if(entry.headToIndexRenameDetails().newFilePath().isEmpty()) {
                    throw CommonException("Failed to find rename new file path");
                }
                if(entry.headToIndexRenameDetails().oldFilePath().isEmpty()) {
                    throw CommonException("Failed to find rename old file path");
                }
                paths.append(entry.headToIndexRenameDetails().newFilePath());
                paths.append(entry.headToIndexRenameDetails().oldFilePath());
            }
            else {
                paths.append(entry.path());
            }
        }
    }
    catch(const CommonException& e)
    {
        logText(LVL_ERROR, e.message());
    }
    return paths;
}

void StatusEntryTableView::onCurrentSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)
    StatusEntry entry = StatusEntry::fromVariant(current.data(StatusEntryRole));
    if(entry.isValid()) {
        emit statusEntryClicked(entry);
    }
}

