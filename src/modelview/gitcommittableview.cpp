#include "gitcommittableview.h"
#include "gitcommittablemodel.h"
#include "gitroles.h"
#include "gitassets.h"

#include <QHeaderView>
#include <QPainter>
#include <git2qt.h>

#include <Kanoop/geometry/rectangle.h>

#include <Kanoop/gui/resources.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

GitCommitTableView::GitCommitTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(SingleSelection);
    verticalHeader()->setDefaultSectionSize(GitCommitGraphStyledItemDelegate::RowHeight);
}

void GitCommitTableView::createModel(Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

    _commits = repo->commitGraph();
    _workInProgress = repo->status().entries();
    GitCommitTableModel* tableModel = new GitCommitTableModel(repo, _commits, this);
    setSourceModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitCommitTableView::onCurrentIndexChanged);

    int column = tableModel->columnForHeader(CH_Graph);
    if(column >= 0) {
        setItemDelegateForColumn(column, new GitCommitGraphStyledItemDelegate(this));
    }

    restoreHeaderStates();
}

QModelIndex GitCommitTableView::findCommit(const GIT::ObjectId& objectId) const
{
    return static_cast<GitCommitTableModel*>(sourceModel())->findCommitIndex(objectId);
}

void GitCommitTableView::selectCommit(const GIT::ObjectId& objectId)
{
    QModelIndex index = findCommit(objectId);
    if(index.isValid()) {
        selectRow(index.row());
    }
}

Stash GitCommitTableView::currentSelectedStash() const
{
    Stash stash;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true && currentMetadataType() == GitEntities::Stash) {
        stash = Stash::fromVariant(index.data(StashRole));
    }
    return stash;
}

GraphedCommit GitCommitTableView::currentSelectedCommit() const
{
    GraphedCommit commit;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true && currentMetadataType() == GitEntities::Commit) {
        commit = GraphedCommit::fromVariant(index.data(CommitRole));
    }
    return commit;
}

GitEntities::Type GitCommitTableView::currentMetadataType() const
{
    GitEntities::Type type = GitEntities::InvalidEntity;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true) {
        type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    }
    return type;
}

void GitCommitTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    GitEntities::Type itemType = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();

    switch (itemType) {
    case GitEntities::Commit:
    {
        GraphedCommit commit = GraphedCommit::fromVariant(current.data(CommitRole));
        if(commit.isNull() == false) {
            emit commitClicked(commit);
        }
        break;
    }
    case GitEntities::WorkInProgress:
        emit workInProgressClicked();
        break;
    case GitEntities::Stash:
    {
        Stash stash = Stash::fromVariant(current.data(StashRole));
        if(stash.isValid()) {
            emit stashClicked(stash);
        }
        break;
    }
    default:
        break;
    }
}

// ------------------------------------ GitCommitGraphStyledItemDelegate ------------------------------------

GitCommitGraphStyledItemDelegate::GitCommitGraphStyledItemDelegate(GitCommitTableView* parent) :
    QStyledItemDelegate(parent),
    _tableView(parent)
{

}

void GitCommitGraphStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Rectangle drawRect = option.rect;
    drawRect.grow(1);

    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    painter->save();
    switch(type) {
    case GitEntities::Commit:
    case GitEntities::Stash:
    {
        GraphedCommit commit = GraphedCommit::fromVariant(index.data(CommitRole));
        if(commit.isValid() == false) {
            return;
        }
        QPixmap pixmap = createCommitPixmap(commit, drawRect.size());
        painter->drawPixmap(drawRect.x(), drawRect.y(), pixmap);
        break;
    }

    case GitEntities::WorkInProgress:
    {
        QPixmap pixmap = createWorkInProgressPixmap(drawRect.size());
        painter->drawPixmap(drawRect.x(), drawRect.y(), pixmap);
        break;
    }

    default:
        break;
    }
    painter->restore();
}

QPixmap GitCommitGraphStyledItemDelegate::createArc(int width, int height, GIT::GraphItemType type)
{

    QPixmap pixmap(width * 2, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setPen(QPen(Colors::blue, 3));

    Rectangle rect = pixmap.rect();
    painter.drawArc(rect, 0, 360*16);

    Rectangle arcRect;
    Size arcSize(rect.width() / 2, rect.height() / 2);
    switch(type) {
    case DownToLeft:
    case RightThenUp:
        arcRect = Rectangle(Point(rect.width() / 2, rect.height() / 2), arcSize);
        break;
    case DownToRight:
    case LeftThenUp:
        arcRect = Rectangle(Point(0, rect.height() / 2), arcSize);
        break;
    case UpToLeft:
    case RightThenDown:
        arcRect = Rectangle(Point(rect.width() / 2, 0), arcSize);
        break;
    case UpToRight:
    case LeftThenDown:
        arcRect = Rectangle(Point(0, 0), arcSize);
        break;
    default:
        break;
    }

    QPixmap result = pixmap.copy(arcRect.toRect());

    return result;
}

QPixmap GitCommitGraphStyledItemDelegate::createArc_DEP(int width, int height, GIT::GraphItemType type)
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    Rectangle levelRect(0, 0, width, height);
    Point dotPos = levelRect.centerPoint();

    Size arcRectSize(width * 2, height);

    int startAngle = 0;
    int spanAngle = 0;

    Rectangle arcRect;
    switch(type) {
    case DownToLeft:
        arcRect = Rectangle(Point(levelRect.x() - (width / 2), dotPos.y()), arcRectSize);
        startAngle = -(270 * 16);
        spanAngle = (90 * 16);
        break;
    case DownToRight:
        arcRect = Rectangle(Point(levelRect.x() - (width / 2), dotPos.y()), arcRectSize);
        startAngle = -(270 * 16);
        spanAngle = -(90 * 16);
        break;
    case UpToLeft:
        arcRect = Rectangle(Point(levelRect.x() - (width / 2), dotPos.y() - height), arcRectSize);
        startAngle = (270 * 16);
        spanAngle = -(90 * 16);
        break;
    case UpToRight:
        arcRect = Rectangle(Point(levelRect.x() - (width / 2), dotPos.y() - height), arcRectSize);
        startAngle = (270 * 16);
        spanAngle = (90 * 16);
        break;
    default:
        break;
    }

    // painter.setPen(Colors::red);
    // painter.drawRect(arcRect);

    painter.setPen(Colors::blue);
    painter.drawArc(arcRect, startAngle, spanAngle);

    return pixmap;
}

QPixmap GitCommitGraphStyledItemDelegate::createCommitPixmap(const GIT::GraphedCommit& commit, const Size& size) const
{
    QPixmap pixmap(size.toSize());
    pixmap.fill();

    QPainter painter(&pixmap);

    GraphLine graphLine = commit.graphLine();
    QList<int> levels = graphLine.graphItems().keys();
    for(int level : levels) {
        GraphItemTypes itemTypes = graphLine.graphItems().value(level);
        if(itemTypes & VerticalUp) {
            drawVertical(&painter, size, level, Geo::Up);
        }
        if(itemTypes & VerticalDown) {
            drawVertical(&painter, size, level, Geo::Down);
        }
        if(itemTypes & UpToLeft) {
            drawCurvedConnector(&painter, size, level, UpToLeft, commit);
        }
        if(itemTypes & UpToRight) {
            drawCurvedConnector(&painter, size, level, UpToRight, commit);
        }
        if(itemTypes & DownToLeft) {
            drawCurvedConnector(&painter, size, level, DownToLeft, commit);
        }
        if(itemTypes & DownToRight) {
            drawCurvedConnector(&painter, size, level, DownToRight, commit);
        }
        if(itemTypes & LeftThenDown) {
            drawCurvedConnector(&painter, size, level, LeftThenDown, commit);
        }
        if(itemTypes & LeftThenUp) {
            drawCurvedConnector(&painter, size, level, LeftThenUp, commit);
        }
        if(itemTypes & RightThenDown) {
            drawCurvedConnector(&painter, size, level, RightThenDown, commit);
        }
        if(itemTypes & RightThenUp) {
            drawCurvedConnector(&painter, size, level, RightThenUp, commit);
        }
        if(itemTypes & HorizontalLeft) {
            drawHorizontal(&painter, size, level, Geo::ToLeft);
        }
        if(itemTypes & HorizontalRight) {
            drawHorizontal(&painter, size, level, Geo::ToRight);
        }
        if(itemTypes & CommitDot) {
            drawCommitDot(&painter, size, commit);
        }
        if(itemTypes & MergeDot) {
            drawMergeDot(&painter, size, commit);
        }
    }

    return pixmap;
}

QPixmap GitCommitGraphStyledItemDelegate::createWorkInProgressPixmap(const Size& size) const
{
    QPixmap pixmap(size.toSize());
    pixmap.fill();

    QPainter painter(&pixmap);

    static const int MARGIN = 2;
    Point centerPoint = centerPointForLevel(1);
    int expand = qMin((size.height() / 2) - MARGIN, (double)(LevelWidth / 2) - MARGIN);
    Rectangle drawRect = Rectangle::fromCenterPoint(centerPoint, expand);

    drawVertical(&painter, size, 1, Geo::Down);

    QPixmap pencil = Resources::getPixmap(GitAssets::Pencil);
    pencil = pencil.scaled(drawRect.size().toSize());
    painter.drawPixmap(drawRect.toRect(), pencil);

    return pixmap;
}

void GitCommitGraphStyledItemDelegate::drawCommitDot(QPainter* painter, const Size& size, const GIT::GraphedCommit& commit) const
{
    Point dotPos(centerXForLevel(commit.level()), size.height() / 2);

    painter->save();
    if(commit.isStash()) {
        painter->setBrush(QBrush(Qt::white));
        QPen pen(Colors::darkmagenta);
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
        painter->setPen(pen);
        Rectangle rect = Rectangle::fromCenterPoint(dotPos, StashRadius);
        painter->drawRect(rect);
    }
    else  {
        painter->setPen(QPen(Colors::green, 2));
        painter->setBrush(QBrush(Colors::yellow));
        painter->drawEllipse(dotPos, DotRadius, DotRadius);
painter->setPen(Colors::red);
painter->drawPoint(dotPos);
    }
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawMergeDot(QPainter *painter, const Size &size, const GIT::GraphedCommit &commit) const
{
    Point dotPos(centerXForLevel(commit.level()), size.height() / 2);

    painter->save();
    painter->setBrush(QBrush(Colors::blue));
    painter->drawEllipse(dotPos, MergeRadius, MergeRadius);
painter->setPen(Colors::red);
painter->drawPoint(dotPos);
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawCurvedConnector(QPainter* painter, const Size& pixmapSize, int level, GraphItemType type, const GraphedCommit& commit) const
{
    Q_UNUSED(commit)

    Point dotPos(centerXForLevel(level), pixmapSize.height() / 2);

    QPixmap arc = createArc(LevelWidth, pixmapSize.height(), type);

    int drawX = 0;
    int drawY = 0;
    switch(type) {
    case LeftThenDown:
        drawX = dotPos.x() - LevelWidth;
        drawY = dotPos.y();
        break;
    case RightThenDown:
        drawX = dotPos.x();
        drawY = dotPos.y();
        break;
    case UpToLeft:
    case DownToLeft:
    case LeftThenUp:
        drawX = dotPos.x() - LevelWidth;
        drawY = dotPos.y() - (RowHeight / 2);
        break;
    case UpToRight:
    case DownToRight:
    case RightThenUp:
        drawX = dotPos.x();
        drawY = dotPos.y() - (RowHeight / 2);
        break;
    default:
        break;
    }

    painter->save();
    painter->setPen(QPen(QBrush(Colors::purple), 2));
    painter->drawPixmap(drawX, drawY, arc);
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawCurvedConnector_DEP(QPainter* painter, const Size& pixmapSize, int fromLevel, int toLevel, bool descending) const
{
    Point dotPos(centerXForLevel(fromLevel), pixmapSize.height() / 2);

    Point hlineEnd(centerXForLevel(toLevel) - (LevelWidth / 2), dotPos.y());
    Line hline(dotPos, hlineEnd);
    painter->save();
    painter->setPen(QPen(QBrush(Colors::purple), 2));
    painter->drawLine(hline.toQLine());

    if(descending) {
        Rectangle arcRect(hlineEnd.x() - (LevelWidth / 2), hlineEnd.y(), LevelWidth, pixmapSize.height() / 2);
        painter->drawArc(arcRect, -(270*16), -(90*16));
        Line remaining(arcRect.rightEdge().midpoint(), arcRect.bottomRight());
        painter->drawLine(remaining.toQLine());
    }
    else {
        Rectangle arcRect(hlineEnd.x() - (LevelWidth / 2), 0, LevelWidth, pixmapSize.height() / 2);
        painter->drawArc(arcRect, -(0*16), -(90*16));
        Line remaining(arcRect.rightEdge().midpoint(), arcRect.topRight());
        painter->drawLine(remaining.toQLine());
    }
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawVerticals(QPainter* painter, const Size& pixmapSize, int level, int index, bool forceFinal) const
{
    if(index == 0 && _tableView->workInProgressRef().count() > 0 && forceFinal == false) {
        drawVertical(painter, pixmapSize, level, Geo::Up);
    }
    if(index != 0) {
        drawVertical(painter, pixmapSize, level, Geo::Up);
    }
    if(index < _tableView->commitsRef().count() - 1) {
        drawVertical(painter, pixmapSize, level, Geo::Down);
    }
}

void GitCommitGraphStyledItemDelegate::drawVertical(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const
{
    Point dotPos(centerXForLevel(level), pixmapSize.height() / 2);

    Line line;
    if(direction == Geo::Up) {
        line = Line(Point(dotPos.x(), 0), dotPos);
    }
    else if(direction == Geo::Down) {
        line = Line(dotPos, Point(dotPos.x(), pixmapSize.height()));
    }

    painter->save();
    painter->setPen(QPen(QBrush(Colors::darkblue), 2));
    painter->drawLine(line.toQLine());
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawHorizontal(QPainter* painter, const Size& pixmapSize, int level, Geo::Direction direction) const
{
    Point dotPos(centerXForLevel(level), pixmapSize.height() / 2);

    Line line;
    if(direction == Geo::ToLeft) {
        line = Line(Point(dotPos.x() - (LevelWidth / 2), dotPos.y()), dotPos);
    }
    else if(direction == Geo::ToRight) {
        line = Line(dotPos, Point(dotPos.x() + (LevelWidth / 2), dotPos.y()));
    }

    painter->save();
    painter->setPen(QPen(QBrush(Colors::darkgreen), 2));
    painter->drawLine(line.toQLine());
    painter->restore();
}


// ------------------------------- GitBranchTagStyledItemDelegate -------------------------------


void GitBranchTagStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Rectangle drawRect = option.rect;

    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    painter->save();
    if(type == GitEntities::Commit) {
        GraphedCommit commit = GraphedCommit::fromVariant(index.data(CommitRole));
        if(commit.isValid() == false) {
            return;
        }
        QPixmap pixmap = createCommitPixmap(commit, drawRect.size());
        painter->drawPixmap(drawRect.x(), drawRect.y(), pixmap);
        break;
    }
    else {

    }

    painter->restore();
}
