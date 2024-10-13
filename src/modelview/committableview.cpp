#include "committableview.h"
#include "committablemodel.h"
#include "kanoopgittypes.h"
#include "gitassets.h"

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <git2qt.h>

#include <Kanoop/geometry/rectangle.h>

#include <Kanoop/gui/resources.h>
#include <Kanoop/gui/stylesheets.h>

#include <widgets/branchtaglabelwidget.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

CommitTableView::CommitTableView(QWidget *parent) :
    TableViewBase(parent),
    _repo(nullptr),
    _editingBranchName(false)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(SingleSelection);
    verticalHeader()->setDefaultSectionSize(RowHeight);

    createPixmaps();
    setEditTriggers(AllEditTriggers);

    connect(this, &CommitTableView::horizontalHeaderChanged, this, &CommitTableView::onHorizontalHeaderChanged);
}

CommitTableView::~CommitTableView()
{
    qDeleteAll(_branchLabelWidgets);
}

void CommitTableView::createModel(Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

logText(LVL_DEBUG, "Point 1-1");

    _repo = repo;
    _commits = repo->commitGraph();
logText(LVL_DEBUG, "Point 1-2");
    StatusOptions options;
    options.setExcludeSubmodules(false);
    options.setShow(StatusShowIndexAndWorkDir);
    _workInProgress = repo->status(options).entries();
logText(LVL_DEBUG, "Point 1-3");
    CommitTableModel* tableModel = new CommitTableModel(repo, _commits, _workInProgress, this);
logText(LVL_DEBUG, "Point 1-4");
    setModel(tableModel);

    connect(tableModel, &CommitTableModel::createBranch, this, &CommitTableView::createBranch);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &CommitTableView::onCurrentIndexChanged);

    int column = tableModel->columnForHeader(CH_Graph);
    if(column >= 0) {
        setItemDelegateForColumn(column, new GitCommitGraphStyledItemDelegate(this));
    }
    column = tableModel->columnForHeader(CH_BranchOrTag);
    if(column >= 0) {
        setItemDelegateForColumn(column, new GitBranchTagStyledItemDelegate(this));
    }

    // Detect detached head
    Commit detachedHeadCommit;
    if(_repo->head().isDetachedHead()) {
        detachedHeadCommit = _repo->headCommit();
    }

    // Create branch/tag widgets
    for(const GraphedCommit& commit : _commits) {
        bool thisIsDetachedHead = detachedHeadCommit.isValid() && commit.objectId() == detachedHeadCommit.objectId();
        if(commit.isHead() == true || thisIsDetachedHead) {
            Reference::List references = _repo->references().findByTargetObjectId(commit.objectId());
            BranchTagLabelWidget* labelWidget = new BranchTagLabelWidget(_repo, references);
            labelWidget->setFixedWidth(300);
            _branchLabelWidgets.insert(commit.objectId(), labelWidget);
        }
    }

    restoreHeaderStates();
}

QModelIndex CommitTableView::findCommit(const GIT::ObjectId& objectId) const
{
    return static_cast<CommitTableModel*>(sourceModel())->findCommitIndex(objectId);
}

void CommitTableView::selectCommit(const GIT::ObjectId& objectId)
{
    QModelIndex index = findCommit(objectId);
    if(index.isValid()) {
        QModelIndex bottomLeft = sourceModel()->index(index.row(), sourceModel()->columnCount() - 1);
        QItemSelection selection(index, bottomLeft);
        selectionModel()->select(selection, QItemSelectionModel::Select);
    }
}

void CommitTableView::selectWorkInProgress()
{
    CommitTableModel* tableModel = static_cast<CommitTableModel*>(sourceModel());
    if(tableModel == nullptr) {
        return;
    }
    QModelIndex index = tableModel->findWorkInProgress();
    if(index.isValid() == false) {
        return;
    }
    QModelIndex bottomLeft = sourceModel()->index(index.row(), sourceModel()->columnCount() - 1);
    QItemSelection selection(index, bottomLeft);
    selectionModel()->select(selection, QItemSelectionModel::Select);
    emit workInProgressClicked();
}

Stash CommitTableView::currentSelectedStash() const
{
    Stash stash;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true && currentMetadataType() == GitEntities::Stash) {
        stash = Stash::fromVariant(index.data(StashRole));
    }
    return stash;
}

GraphedCommit CommitTableView::currentSelectedCommit() const
{
    GraphedCommit commit;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true && currentMetadataType() == GitEntities::Commit) {
        commit = GraphedCommit::fromVariant(index.data(CommitRole));
    }
    return commit;
}

int CommitTableView::selectedCount() const
{
    int result = selectedIndexes().count();
    return result;
}

bool CommitTableView::hasWorkInProgress() const
{
    CommitTableModel* tableModel = static_cast<CommitTableModel*>(sourceModel());
    if(tableModel != nullptr) {
        return tableModel->findWorkInProgress().isValid();
    }
    return false;
}

GitEntities::Type CommitTableView::currentMetadataType() const
{
    GitEntities::Type type = GitEntities::InvalidEntity;
    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid() == true) {
        type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    }
    return type;
}

void CommitTableView::createPixmaps()
{
    _cloudPixmap = Resources::getPixmap(GitAssets::Cloud);
    _computerPixmap = Resources::getPixmap(GitAssets::Computer);
    Size pixmapSize(RowHeight / 2, RowHeight / 2);
    _cloudPixmap = _cloudPixmap.scaled(pixmapSize.toSize());
    _computerPixmap = _computerPixmap.scaled(pixmapSize.toSize());
}

void CommitTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
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

void CommitTableView::onHorizontalHeaderChanged()
{
    int width = horizontalHeader()->sectionSize(0);
    QList<BranchTagLabelWidget*> widgets = _branchLabelWidgets.values();
    for(BranchTagLabelWidget* widget : widgets) {
        widget->setFixedWidth(width);
    }
}

// ------------------------------------ GitCommitGraphStyledItemDelegate ------------------------------------

GitCommitGraphStyledItemDelegate::GitCommitGraphStyledItemDelegate(CommitTableView* parent) :
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

    bool isRepoHead = index.data(IsRepoHeadCommitRole).toBool();
    switch(type) {
    case GitEntities::Commit:
    case GitEntities::Stash:
    {
        GraphedCommit commit = GraphedCommit::fromVariant(index.data(CommitRole));
        if(commit.isValid() == false) {
            return;
        }
        QPixmap pixmap = createCommitPixmap(commit, drawRect.size(), isRepoHead, option);
        painter->drawPixmap(drawRect.x(), drawRect.y(), pixmap);
        break;
    }

    case GitEntities::WorkInProgress:
    {
        QPixmap pixmap = createWorkInProgressPixmap(drawRect.size(), option);
        painter->drawPixmap(drawRect.x(), drawRect.y(), pixmap);
        break;
    }

    default:
        break;
    }
    painter->restore();
}

QPixmap GitCommitGraphStyledItemDelegate::createArc(int width, int height, GIT::GraphItemType type, const GitGraphPalette &palette)
{

    QPixmap pixmap(width * 2, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setPen(QPen(palette.graphLineColor(), 2));

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

QPixmap GitCommitGraphStyledItemDelegate::createCommitPixmap(const GIT::GraphedCommit& commit, const Size& size, bool isRepoHead, const QStyleOptionViewItem& option) const
{
    QPixmap pixmap(size.toSize());
    pixmap.fill();

    if(option.state & QStyle::State_Selected) {
        pixmap.fill(option.palette.color(QPalette::Highlight));
    }

    QPainter painter(&pixmap);

    if(isRepoHead) {
        Rectangle rect = pixmap.rect();
        Point begin(rect.left(), rect.rightEdge().midpoint().y());
        Point end(rect.width(), begin.y());
        painter.setPen(QPen(_palette.headCommitLineColor(), _palette.headCommitLineWidth()));
        painter.drawLine(begin, end);
    }

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

QPixmap GitCommitGraphStyledItemDelegate::createWorkInProgressPixmap(const Size& size, const QStyleOptionViewItem& option) const
{
    QPixmap pixmap(size.toSize());

    if(option.state & QStyle::State_Selected) {
        pixmap.fill(option.palette.color(QPalette::Highlight));
    }
    else {
        pixmap.fill();
    }

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
        painter->setBrush(QBrush(_palette.stashFillColor()));
        QPen pen(_palette.stashBorderColor());
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
        painter->setPen(pen);
        Rectangle rect = Rectangle::fromCenterPoint(dotPos, StashRadius);
        painter->drawRect(rect);
    }
    else  {
        painter->setPen(QPen(_palette.commitDotBorderColor(), 2));
        painter->setBrush(QBrush(_palette.commitDotFillColor()));
        painter->drawEllipse(dotPos, DotRadius, DotRadius);
    }
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawMergeDot(QPainter *painter, const Size &size, const GIT::GraphedCommit &commit) const
{
    Point dotPos(centerXForLevel(commit.level()), size.height() / 2);

    painter->save();
    painter->setBrush(QBrush(_palette.mergeDotColor()));
    painter->drawEllipse(dotPos, MergeRadius, MergeRadius);
    painter->restore();
}

void GitCommitGraphStyledItemDelegate::drawCurvedConnector(QPainter* painter, const Size& pixmapSize, int level, GraphItemType type, const GraphedCommit& commit) const
{
    Q_UNUSED(commit)

    Point dotPos(centerXForLevel(level), pixmapSize.height() / 2);

    QPixmap arc = createArc(LevelWidth, pixmapSize.height(), type, _palette);

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
        drawY = dotPos.y() - (CommitTableView::RowHeight / 2);
        break;
    case UpToRight:
    case DownToRight:
    case RightThenUp:
        drawX = dotPos.x();
        drawY = dotPos.y() - (CommitTableView::RowHeight / 2);
        break;
    default:
        break;
    }

    painter->save();
    // painter->setPen(QPen(QBrush(Colors::purple), 2));
    painter->drawPixmap(drawX, drawY, arc);
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

    // Draw the label
    QString text = index.data(Qt::DisplayRole).toString();
    if(text.isEmpty() == true) {
        return;
    }

    GraphedCommit commit = GraphedCommit::fromVariant(index.data(CommitRole));
    if(commit.isNull()) {
        return;
    }

    BranchTagLabelWidget* widget = _tableView->getBranchLabelWidget(commit.objectId());
    if(widget == nullptr) {
        return;
    }

    Rectangle rect = option.rect;
    painter->save();

    painter->translate(rect.topLeft());
    widget->resize(rect.size().toSize());
    widget->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}

QWidget *GitBranchTagStyledItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    QWidget* result = nullptr;

    if(_tableView->isEditingBranchName()) {
        result = createBranchNameEditor(parent, index);
    }
    else {
        result = getBranchLabelWidget(parent, index);
    }

    return result;
}

void GitBranchTagStyledItemDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(index)
    if(dynamic_cast<BranchTagLabelWidget*>(editor) == nullptr) {
        delete editor;
    }
}

void GitBranchTagStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model) Q_UNUSED(index)
    if(_tableView->isEditingBranchName()) {
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
        if(lineEdit == nullptr) {
            return;     // shouldn't happen
        }

        if(lineEdit->text().isEmpty() == false) {
            model->setData(index, lineEdit->text(), CreateBranchRole);
        }
        _tableView->setEditingBranchName(false);
    }
    else {
        Log::logText(LVL_DEBUG, "Do something here");
    }
}

QLineEdit* GitBranchTagStyledItemDelegate::createBranchNameEditor(QWidget *parent, const QModelIndex &index) const
{
    bool isHeadCommit = index.data(IsRepoHeadCommitRole).toBool();
    if(isHeadCommit == false) {
        return nullptr;     // shouldn't happen
    }

    QLineEdit* editor = new QLineEdit(parent);
    editor->setPlaceholderText("-- branch name --");
    return editor;
}

BranchTagLabelWidget* GitBranchTagStyledItemDelegate::getBranchLabelWidget(QWidget* parent, const QModelIndex& index) const
{
    BranchTagLabelWidget* result = nullptr;
    GraphedCommit commit = GraphedCommit::fromVariant(index.data(CommitRole));
    if(commit.isNull() == false && (result = _tableView->getBranchLabelWidget(commit.objectId())) != nullptr) {
        result->setParent(parent);
    }

    return result;
}

