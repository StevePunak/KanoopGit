#ifndef COMMITTABLEVIEW_H
#define COMMITTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>
#include <QStyledItemDelegate>
#include <Kanoop/geometry/point.h>
#include <Kanoop/geometry/size.h>
#include "gitentities.h"
#include "gitgraphpalette.h"
#include "kanoopgittypes.h"

class BranchTagLabelWidget;
class CommitTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit CommitTableView(QWidget* parent = nullptr);
    virtual ~CommitTableView();

    void createModel(GIT::Repository* repo);

    QModelIndex findCommit(const GIT::ObjectId& objectId) const;
    void selectCommit(const GIT::ObjectId& objectId);
    void selectWorkInProgress();

    GIT::Stash currentSelectedStash() const;
    GIT::GraphedCommit currentSelectedCommit() const;

    int selectedCount() const;
    bool hasWorkInProgress() const;

    GitEntities::Type currentMetadataType() const;

    const GIT::GraphedCommit::List commitsRef() { return _commits; }
    const GIT::GraphedCommit::Map commitIndexRef() { return _commitIndex; }
    const GIT::StatusEntry::List workInProgressRef() { return _workInProgress; }

    bool isEditingBranchName() const { return _editingBranchName; }
    void setEditingBranchName(bool value) { _editingBranchName = value; }

    QPixmap cloudPixmap() const { return _cloudPixmap; }
    QPixmap computerPixmap() const { return _computerPixmap; }

    static const int RowHeight = 30;

private:
    void createPixmaps();

    // Calls for friend class
    BranchTagLabelWidget* getBranchLabelWidget(const GIT::ObjectId& objectId) const { return _branchLabelWidgets.value(objectId); }

    GIT::Repository* _repo;
    bool _editingBranchName;

    GIT::GraphedCommit::List _commits;
    GIT::GraphedCommit::Map _commitIndex;
    GIT::StatusEntry::List _workInProgress;

    QPixmap _cloudPixmap;
    QPixmap _computerPixmap;

    GitGraphPalette _graphPalette;

    QMap<GIT::ObjectId, BranchTagLabelWidget*> _branchLabelWidgets;

    friend class GitBranchTagStyledItemDelegate;

signals:
    void commitClicked(const GIT::GraphedCommit& commit);
    void workInProgressClicked();
    void stashClicked(const GIT::Stash& stash);
    void createBranch(const QString& branchName);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void onHorizontalHeaderChanged();
};

class GitCommitGraphStyledItemDelegate : public QStyledItemDelegate
{
public:
    GitCommitGraphStyledItemDelegate(CommitTableView* parent = nullptr);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    static QPixmap createArc(int width, int height, GIT::GraphItemType type, const GitGraphPalette& palette);

private:
    QPixmap createCommitPixmap(const GIT::GraphedCommit& commit, const Size& size, bool isRepoHead, const QStyleOptionViewItem& option) const;
    QPixmap createWorkInProgressPixmap(const Size& size, const QStyleOptionViewItem& option) const;
    void drawCommitDot(QPainter* painter, const Size& size, const GIT::GraphedCommit& commit) const;
    void drawMergeDot(QPainter* painter, const Size& size, const GIT::GraphedCommit& commit) const;
    void drawCurvedConnector(QPainter* painter, const Size& pixmapSize, int level, GIT::GraphItemType type, const GIT::GraphedCommit& commit) const;
    void drawVerticals(QPainter* painter, const Size& pixmapSize, int level, int index, bool forceFinal = false) const;
    void drawVertical(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const;
    void drawHorizontal(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const;

    static double centerXForLevel(int level) { return (LevelWidth / 2) + (LevelWidth * (level - 1)); }
    static Point centerPointForLevel(int level) { return Point(centerXForLevel(level), CommitTableView::RowHeight / 2); }

    CommitTableView* _tableView;
    GitGraphPalette _palette;

public:
    static const int LevelWidth = 16;
    static const int DotRadius = 6;
    static const int MergeRadius = 4;
    static const int StashRadius = 6;
};

class GitBranchTagStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    GitBranchTagStyledItemDelegate(CommitTableView* parent = nullptr) :
        QStyledItemDelegate(parent),
        _tableView(parent) {}

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    QLineEdit* createBranchNameEditor(QWidget *parent, const QModelIndex &index) const;
    BranchTagLabelWidget* getBranchLabelWidget(QWidget *parent, const QModelIndex &index) const;

    CommitTableView* _tableView;
    GitGraphPalette _palette;
};

#endif // COMMITTABLEVIEW_H
