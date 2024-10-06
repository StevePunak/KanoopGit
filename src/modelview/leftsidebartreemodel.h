#ifndef LEFTSIDEBARTREEMODEL_H
#define LEFTSIDEBARTREEMODEL_H
#include <Kanoop/gui/abstracttreemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <QFont>
#include <git2qt.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class LeftSidebarTreeModel : public AbstractTreeModel
{
    Q_OBJECT
public:
    LeftSidebarTreeModel(GIT::Repository* repo, QObject* parent = nullptr);

    QModelIndex localBranchesIndex() const;
    QModelIndex remoteBranchesIndex() const;
    QModelIndex submodulesIndex() const;

    QModelIndexList localBranchIndexes() const;
    QModelIndexList remoteBranchIndexes() const;
    QModelIndexList submoduleIndexes() const;

private:
    void createLocalBranchesLeaf();
    void createRemoteBranchesLeaf();
    void createSubmodulesLeaf();

    void loadBranches(AbstractModelItem* rootItem, const GIT::Reference::List& references, bool local);
    void ensureFolder(AbstractModelItem* rootItem, const QString& path);

    QModelIndex findBranchIndex(AbstractModelItem* rootItem, const QString& path) const;
    QModelIndex findReferenceIndex(AbstractModelItem* rootItem, const GIT::ObjectId& objectId) const;
    QModelIndex findFolderIndex(AbstractModelItem* rootItem, const QString& path) const;
    QModelIndex findRemoteIndex(AbstractModelItem* rootItem, const QString& remoteName) const;
    QModelIndex findTitleItemIndex(AbstractModelItem* item) const;

    static bool isRootPath(const QString& path) { return path.contains('/') == false; }
    static QString parentPath(const QString& path);

    GIT::Repository* _repo;

    class TreeBaseItem : public AbstractModelItem
    {
    protected:
        TreeBaseItem(const EntityMetadata& metadata, LeftSidebarTreeModel* model) :
            AbstractModelItem(metadata, model) {}

        GIT::Repository* repo() { return static_cast<LeftSidebarTreeModel*>(model())->_repo; }
    };

    class TitleItem : public TreeBaseItem
    {
    public:
        explicit TitleItem(const QString& text, const QIcon& icon, ControlType controlType, LeftSidebarTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _title;
        QFont _font;
        QIcon _icon;
        ControlType _controlType;
    };

    class SubmoduleItem : public TreeBaseItem
    {
    public:
        explicit SubmoduleItem(const GIT::Submodule& submodule, LeftSidebarTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::Submodule _submodule;
    };

    class FolderItem : public TreeBaseItem
    {
    public:
        explicit FolderItem(const QString& path, LeftSidebarTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _path;
        QString _folderName;
    };

    class ReferenceItem : public TreeBaseItem
    {
    protected:
        explicit ReferenceItem(const GIT::Reference& reference, LeftSidebarTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        GIT::Reference _reference;
        QString _text;
        bool _isCurrentBranch;
    };

    class LocalBranchItem : public ReferenceItem
    {
    public:
        explicit LocalBranchItem(const GIT::Reference& reference, LeftSidebarTreeModel* model) :
            ReferenceItem(reference, model) {}
    };

    class RemoteBranchItem : public ReferenceItem
    {
    public:
        explicit RemoteBranchItem(const GIT::Reference& reference, LeftSidebarTreeModel* model) :
            ReferenceItem(reference, model) {}
    };
};

#endif // LEFTSIDEBARTREEMODEL_H
