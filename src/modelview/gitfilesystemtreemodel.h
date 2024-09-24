#ifndef GITFILESYSTEMTREEMODEL_H
#define GITFILESYSTEMTREEMODEL_H
#include <Kanoop/gui/abstracttreemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <git2qt.h>

class GitFileSystemTreeModel : public AbstractTreeModel
{
    Q_OBJECT
public:
    GitFileSystemTreeModel(GIT::Repository* repo, QObject* parent = nullptr);

public slots:
    void refresh();

private:
    void loadDirectories();
    void loadFiles();
    QModelIndex findFolderIndex(const QString& path);

    static QString parentPath(const QString& path);

    GIT::Repository* _repo;

    class TreeBaseItem : public AbstractModelItem
    {
    public:
        explicit TreeBaseItem(const EntityMetadata& metadata, const QString& path, GitFileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    protected:
        GIT::Repository* repo() const { return static_cast<GitFileSystemTreeModel*>(model())->_repo; }
        QString absolutePath() const { return _absolutePath; }
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
        explicit FolderItem(const QString& path, GitFileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _folderName;
        QIcon _icon;
    };

    class FileItem : public TreeBaseItem
    {
    public:
        explicit FileItem(const QString& path, GitFileSystemTreeModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;
    };
};

#endif // GITFILESYSTEMTREEMODEL_H
