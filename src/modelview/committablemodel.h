#ifndef COMMITTABLEMODEL_H
#define COMMITTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class CommitTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    CommitTableModel(GIT::Repository* repo, const GIT::GraphedCommit::List& commits, const GIT::StatusEntry::List statusEntries, QObject* parent = nullptr);

    QModelIndex findCommitIndex(const GIT::ObjectId& objectId) const;
    QModelIndex findWorkInProgress() const;

protected:
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

signals:
    void createBranch(const QString& branchName);

private:
    GIT::Repository* _repo;

    class TableBaseItem : public AbstractModelItem
    {
    public:
        explicit TableBaseItem(const EntityMetadata& metadata, CommitTableModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<CommitTableModel*>(model())->_repo; }
    };

    class CommitItem : public TableBaseItem
    {
    public:
        explicit CommitItem(const GIT::GraphedCommit& commit, CommitTableModel* model);

    protected:
        explicit CommitItem(const EntityMetadata& metadata, const GIT::GraphedCommit& commit, CommitTableModel* model) :
            TableBaseItem(metadata, model),
            _commit(commit),
            _isHeadCommit(false),
            _isDetachedHeadCommit(false) {}


        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::GraphedCommit _commit;
        bool _isHeadCommit;
        bool _isDetachedHeadCommit;
    };

    class WorkInProgressItem : public TableBaseItem
    {
    public:
        explicit WorkInProgressItem(const GIT::StatusEntry::List& statusEntries, CommitTableModel* model) :
            TableBaseItem(EntityMetadata(GitEntities::WorkInProgress), model),
            _statusEntries(statusEntries) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::StatusEntry::List _statusEntries;
    };

    class StashItem : public CommitItem
    {
    public:
        explicit StashItem(const GIT::GraphedCommit& commit, const GIT::Stash& stash, CommitTableModel* model) :
            CommitItem(EntityMetadata(GitEntities::Stash), commit, model),
            _stash(stash) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::Stash _stash;
    };
};

#endif // COMMITTABLEMODEL_H
