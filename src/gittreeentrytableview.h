#ifndef GITTREEENTRYTABLEVIEW_H
#define GITTREEENTRYTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class GitTreeEntryTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit GitTreeEntryTableView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo, const GIT::ObjectId& commitId);

signals:
    void treeChangeEntryClicked(const GIT::TreeChangeEntry& treeChangeEntry);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // GITTREEENTRYTABLEVIEW_H
