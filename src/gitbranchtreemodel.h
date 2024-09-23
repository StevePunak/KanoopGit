#ifndef GITBRANCHTREEMODEL_H
#define GITBRANCHTREEMODEL_H
#include <Kanoop/gui/abstracttreemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>

class GitBranchTreeModel : public AbstractTreeModel
{
    Q_OBJECT
public:
    GitBranchTreeModel(GIT::Repository* repo, GIT::BranchType typeToShow, QObject* parent = nullptr);

public slots:
    void refresh();

private:
    void loadBranches(const GIT::Reference::List& references);
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
        explicit TreeBaseItem(const EntityMetadata& metadata, GitBranchTreeModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<GitBranchTreeModel*>(model())->_repo; }
    };

    class FolderItem : public TreeBaseItem
    {
    public:
        explicit FolderItem(const QString& path, GitBranchTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _path;
        QString _folderName;
    };

    class ReferenceItem : public TreeBaseItem
    {
    public:
        explicit ReferenceItem(const GIT::Reference& reference, GitBranchTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::Reference _reference;
        QString _text;
    };
};

#endif // GITBRANCHTREEMODEL_H
