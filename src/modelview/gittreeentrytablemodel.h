#ifndef GITTREEENTRYTABLEMODEL_H
#define GITTREEENTRYTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class GitTreeEntryTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    GitTreeEntryTableModel(GIT::Repository* repo, const GIT::ObjectId& commitId, QObject* parent = nullptr);

private:
    GIT::Repository* _repo;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, GitTreeEntryTableModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<GitTreeEntryTableModel*>(model())->_repo; }
    };

    class TreeChangeEntryItem : public TreeBaseItem
    {
    public:
        explicit TreeChangeEntryItem(const GIT::TreeChangeEntry& treeChangeEntry, GitTreeEntryTableModel* model) :
            TreeBaseItem(EntityMetadata(GitEntities::IndexEntry), model),
            _treeChangeEntry(treeChangeEntry) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::TreeChangeEntry _treeChangeEntry;
    };
};

#endif // GITTREEENTRYTABLEMODEL_H
