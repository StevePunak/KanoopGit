#ifndef BRANCHTREEMODEL_H
#define BRANCHTREEMODEL_H
#include <Kanoop/gui/abstracttreemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>

class BranchTreeModel : public AbstractTreeModel
{
    Q_OBJECT
public:
    BranchTreeModel(GIT::Repository* repo, GIT::BranchType typeToShow, QObject* parent = nullptr);

public slots:
    void refresh();

private:
    void loadBranches(const GIT::ReferenceList& references);
    void ensureFolder(const QString& path);
    QModelIndex findBranchIndex(const QString& path) const;
    QModelIndex findReferenceIndex(const GIT::ObjectId& objectId) const;
    QModelIndex findFolderIndex(const QString& path) const;
    QModelIndex findRemoteIndex(const QString& remoteName) const;

    static bool isRootPath(const QString& path) { return path.contains('/') == false; }
    static QString parentPath(const QString& path);

    GIT::Repository* _repo;
    GIT::BranchType _typeToShow;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, BranchTreeModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<BranchTreeModel*>(model())->_repo; }
    };

    class FolderItem : public TreeBaseItem
    {
    public:
        explicit FolderItem(const QString& path, BranchTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _path;
        QString _folderName;
    };

    class ReferenceItem : public TreeBaseItem
    {
    public:
        explicit ReferenceItem(const GIT::Reference& reference, BranchTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::Reference _reference;
        QString _text;
        bool _isCurrentBranch;
    };
};

#endif // BRANCHTREEMODEL_H
