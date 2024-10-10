#ifndef RECENTREPOSTABLEVIEW_H
#define RECENTREPOSTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class RecentReposTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit RecentReposTableView(QWidget* parent = nullptr);

    void createModel(const QStringList& repoPaths);

signals:
    void recentRepoPathClicked(const QString& repoPath);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // RECENTREPOSTABLEVIEW_H
