#ifndef FILESYSTEMTREEVIEW_H
#define FILESYSTEMTREEVIEW_H
#include <Kanoop/gui/treeviewbase.h>
#include <git2qt.h>

namespace GIT {
class Repository;
}

class FileSystemTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit FileSystemTreeView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo);

signals:
    void folderClicked(const QString& folderPath);
    void indexEntryClicked(const GIT::IndexEntry& indexEntry);
    void refresh();

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // FILESYSTEMTREEVIEW_H
