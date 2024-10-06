#include "gitassets.h"
#include "committablemodel.h"
#include "kanoopgittypes.h"
#include <QFileInfo>

#include <Kanoop/datetimeutil.h>
#include <Kanoop/pathutil.h>

#include <QFont>

#include <Kanoop/gui/resources.h>

using namespace GIT;

CommitTableModel::CommitTableModel(Repository* repo, const GraphedCommit::List& commits, QObject *parent) :
    AbstractTableModel("committable", parent),
    _repo(repo)
{
    appendColumnHeader(CH_BranchOrTag, "Branch/Tag");
    appendColumnHeader(CH_Graph, "Graph");
    appendColumnHeader(CH_Message, "Message");
    appendColumnHeader(CH_Timestamp, "Timestamp");
    appendColumnHeader(CH_SHA, "SHA");

    StatusEntry::List statusEntries = _repo->status().entries();
    if(statusEntries.count() > 0) {
        appendRootItem(new WorkInProgressItem(statusEntries, this));
    }

    for(const GraphedCommit& commit : commits) {
        if(commit.isStash()) {
            Stash stash = _repo->findStash(commit.objectId());
            if(stash.isValid()) {
                appendRootItem(new StashItem(commit, stash, this));
            }
        }
        else {
            appendRootItem(new CommitItem(commit, this));
        }
    }
}

QModelIndex CommitTableModel::findCommitIndex(const GIT::ObjectId& objectId) const
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, ObjectIdRole, objectId.toVariant(), 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

Qt::ItemFlags CommitTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = AbstractTableModel::flags(index);
    TableHeader headerInfo = columnHeader(index.column());

    if(headerInfo.type() == ColHeaders::CH_BranchOrTag) {
        result |= Qt::ItemIsEditable;
    }
    return result;
}

bool CommitTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index)
    if(role == CreateBranchRole) {
        emit createBranch(value.toString());
    }
    return false;
}

CommitTableModel::CommitItem::CommitItem(const GIT::GraphedCommit &commit, CommitTableModel *model) :
    TableBaseItem(EntityMetadata(GitEntities::Commit), model),
    _commit(commit), _isHeadCommit(false), _isDetachedHeadCommit(false)
{
    if(commit.objectId() == repo()->headCommit().objectId()) {
        _isHeadCommit = true;
    }

    if(repo()->head().isDetachedHead() && commit.objectId() == repo()->headCommit().objectId()) {
        _isDetachedHeadCommit = true;
    }
}

QVariant CommitTableModel::CommitItem::data(const QModelIndex &index, int role) const
{
    QVariant result;
    switch(role) {
    case Qt::DisplayRole:
    {
        TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
        switch(header.type()) {
        case CH_BranchOrTag:
            if(_commit.isHead()) {
                result = _commit.friendlyBranchName();
            }
            else if(_isDetachedHeadCommit) {
                result = "HEAD";
            }
            break;
        case CH_Message:
            result = _commit.shortMessage();
            break;
        case CH_Timestamp:
            result = DateTimeUtil::toStandardString(_commit.timestamp(), false);
            break;
        case CH_SHA:
            result = _commit.objectId().toString();
            break;
        default:
            break;
        }
        break;
    }
    case ObjectIdRole:
        result = _commit.objectId().toVariant();
        break;
    case IsRepoHeadCommitRole:
        result = _isHeadCommit;
        break;
    case CommitRole:
        result = _commit.toVariant();
        break;
    case Qt::DecorationRole:
        break;
    default:
        break;
    }

    if(result.isValid() == false) {
        result = TableBaseItem::data(index, role);
    }
    return result;
}

QVariant CommitTableModel::WorkInProgressItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
    switch(role) {
    case Qt::DisplayRole:
    {
        switch(header.type()) {
        case CH_Message:
            result = QString("%1 modifications").arg(_statusEntries.count());
            break;
        default:
            break;
        }
        break;
    }
    case Qt::DecorationRole:
        switch(header.type()) {
        case CH_Message:
            result = Resources::getIcon(GitAssets::Pencil);
            break;
        default:
            break;
        }
        break;
    case Qt::FontRole:
    {
        QFont font;
        font.setItalic(true);
        result = font;
        break;
    }
    default:
        break;
    }

    if(result.isValid() == false) {
        result = TableBaseItem::data(index, role);
    }
    return result;
}


QVariant CommitTableModel::StashItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
    switch(role) {
    case Qt::DisplayRole:
    {
        switch(header.type()) {
        case CH_Message:
            result = _stash.message();
            break;
        default:
            break;
        }
        break;
    }
    case Qt::DecorationRole:
        switch(header.type()) {
        case CH_Message:
            result = Resources::getIcon(GitAssets::Stash);
            break;
        default:
            break;
        }
        break;
    case Qt::FontRole:
    {
        QFont font;
        font.setItalic(true);
        result = font;
        break;
    }
    case StashRole:
        result = _stash.toVariant();
        break;
    default:
        break;
    }

    if(result.isValid() == false) {
        result = CommitItem::data(index, role);
    }
    return result;
}
