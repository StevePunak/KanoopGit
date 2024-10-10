#ifndef STATUSENTRYTABLEMODEL_H
#define STATUSENTRYTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class StatusEntryTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    StatusEntryTableModel(GIT::Repository* repo, const GIT::StatusEntry::List& statusEntries, QObject* parent = nullptr);

private:
    GIT::Repository* _repo;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, StatusEntryTableModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<StatusEntryTableModel*>(model())->_repo; }
    };

    class StatusEntryItem : public TreeBaseItem
    {
    public:
        explicit StatusEntryItem(const GIT::StatusEntry& statusEntry, StatusEntryTableModel* model) :
            TreeBaseItem(EntityMetadata(GitEntities::Commit), model),
            _statusEntry(statusEntry) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::StatusEntry _statusEntry;
    };
};

#endif // STATUSENTRYTABLEMODEL_H
