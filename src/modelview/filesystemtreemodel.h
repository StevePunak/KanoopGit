#ifndef FILESYSTEMTREEMODEL_H
#define FILESYSTEMTREEMODEL_H
#include <Kanoop/gui/abstracttreemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>

class FileSystemTreeModel : public AbstractTreeModel
{
    Q_OBJECT
public:
    FileSystemTreeModel(GIT::Repository* repo, QObject* parent = nullptr);

public slots:
    void refresh();

private:
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    QModelIndex findFolderIndex(const QString& path);

    static QString parentPath(const QString& path);

    GIT::Repository* _repo;
    bool _rootLazyLoadComplete;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, const QString& path, FileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

        QString absolutePath() const { return _absolutePath; }

    protected:
        GIT::Repository* repo() const { return static_cast<FileSystemTreeModel*>(model())->_repo; }
        QString relativePath() const { return _relativePath; }
        QString filename() const { return _filename; }

    private:
        QString _absolutePath;
        QString _relativePath;
        QString _filename;
    };

    class FolderItem : public TreeBaseItem
    {
    public:
        explicit FolderItem(const QString& path, FileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

        bool lazyLoadComplete() const { return _lazyLoadComplete; }
        void setLazyLoadComplete(bool value);

    private:
        QString _folderName;
        QIcon _icon;
        bool _lazyLoadComplete;
    };

    class FileItem : public TreeBaseItem
    {
    public:
        explicit FileItem(const QString& path, FileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;
    };

    class PlaceHolderItem : public TreeBaseItem
    {
    public:
        explicit PlaceHolderItem(const QString& path, FileSystemTreeModel* model);
    };

    void clearPlaceHolder(FolderItem* item);
    void loadDirectories(const QString& parentDirectory, FolderItem* parentItem);
    void loadFiles(const QString& parentDirectory, FolderItem* parentItem);
};

#endif // FILESYSTEMTREEMODEL_H
