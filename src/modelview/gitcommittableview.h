#ifndef GITCOMMITTABLEVIEW_H
#define GITCOMMITTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>
#include <QStyledItemDelegate>
#include <Kanoop/geometry/point.h>
#include <Kanoop/geometry/size.h>
#include "gitentities.h"
#include "gitroles.h"

class GitCommitTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit GitCommitTableView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo);

    QModelIndex findCommit(const GIT::ObjectId& objectId) const;
    void selectCommit(const GIT::ObjectId& objectId);

    GIT::Stash currentSelectedStash() const;
    GIT::GraphedCommit currentSelectedCommit() const;

    GitEntities::Type currentMetadataType() const;

    const GIT::GraphedCommit::List commitsRef() { return _commits; }
    const GIT::GraphedCommit::Map commitIndexRef() { return _commitIndex; }
    const GIT::StatusEntry::List workInProgressRef() { return _workInProgress; }

private:
    GIT::GraphedCommit::List _commits;
    GIT::GraphedCommit::Map _commitIndex;
    GIT::StatusEntry::List _workInProgress;

signals:
    void commitClicked(const GIT::GraphedCommit& commit);
    void workInProgressClicked();
    void stashClicked(const GIT::Stash& stash);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

class GitCommitGraphStyledItemDelegate : public QStyledItemDelegate
{
public:
    GitCommitGraphStyledItemDelegate(GitCommitTableView* parent = nullptr);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    static QPixmap createArc(int width, int height, GIT::GraphItemType type);
    static QPixmap createArc_DEP(int width, int height, GIT::GraphItemType type);

private:
    QPixmap createCommitPixmap(const GIT::GraphedCommit& commit, const Size& size) const;
    QPixmap createWorkInProgressPixmap(const Size& size) const;
    void drawCommitDot(QPainter* painter, const Size& size, const GIT::GraphedCommit& commit) const;
    void drawMergeDot(QPainter* painter, const Size& size, const GIT::GraphedCommit& commit) const;
    void drawCurvedConnector(QPainter* painter, const Size& pixmapSize, int level, GIT::GraphItemType type, const GIT::GraphedCommit& commit) const;
    void drawCurvedConnector_DEP(QPainter* painter, const Size& pixmapSize, int fromLevel, int toLevel, bool descending) const;
    void drawVerticals(QPainter* painter, const Size& pixmapSize, int level, int index, bool forceFinal = false) const;
    void drawVertical(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const;
    void drawHorizontal(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const;

    static double centerXForLevel(int level) { return (LevelWidth / 2) + (LevelWidth * (level - 1)); }
    static Point centerPointForLevel(int level) { return Point(centerXForLevel(level), RowHeight / 2); }

    GitCommitTableView* _tableView;

public:
    static const int RowHeight = 30;
    static const int LevelWidth = 16;
    static const int DotRadius = 6;
    static const int MergeRadius = 4;
    static const int StashRadius = 6;
};

#endif // GITCOMMITTABLEVIEW_H
