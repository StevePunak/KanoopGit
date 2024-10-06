#ifndef TREEENTRYTABLEMODEL_H
#define TREEENTRYTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class TreeEntryTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    TreeEntryTableModel(GIT::Repository* repo, const GIT::ObjectId& commitId, QObject* parent = nullptr);

private:
    GIT::Repository* _repo;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, TreeEntryTableModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<TreeEntryTableModel*>(model())->_repo; }
    };

    class TreeChangeEntryItem : public TreeBaseItem
    {
    public:
        explicit TreeChangeEntryItem(const GIT::TreeChangeEntry& treeChangeEntry, TreeEntryTableModel* model) :
            TreeBaseItem(EntityMetadata(GitEntities::IndexEntry), model),
            _treeChangeEntry(treeChangeEntry) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::TreeChangeEntry _treeChangeEntry;
    };
};

#endif // TREEENTRYTABLEMODEL_H
