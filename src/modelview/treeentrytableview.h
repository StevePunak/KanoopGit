#ifndef TREEENTRYTABLEVIEW_H
#define TREEENTRYTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class TreeEntryTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit TreeEntryTableView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo, const GIT::ObjectId& commitId);

signals:
    void treeChangeEntryClicked(const GIT::TreeChangeEntry& treeChangeEntry);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // TREEENTRYTABLEVIEW_H
