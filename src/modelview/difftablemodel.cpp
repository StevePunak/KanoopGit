#include "difftablemodel.h"
#include "gitentities.h"
#include "kanoopgittypes.h"
#include <QFileInfo>

#include <Kanoop/fileutil.h>
#include <Kanoop/pathutil.h>
#include <Kanoop/stringutil.h>

#include <QFont>

#include <Kanoop/commonexception.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

// --------------------- Tree to Tree Signature

DiffTableModel::DiffTableModel(Repository* repo, const TreeEntry& oldEntry, const TreeEntry& newEntry, const DiffDelta& delta, QObject *parent) :
    AbstractTableModel("difftable", parent),
    _repo(repo)
{
    commonInit(delta);

    try
    {
        if(oldEntry.isValid() == false || newEntry.isValid() == false) {
            throw CommonException("No tree entry found");
        }

        Blob oldBlob = _repo->findBlob(oldEntry.targetObjectId());
        Blob newBlob = _repo->findBlob(newEntry.targetObjectId());
        if(oldBlob.isValid() == false || newBlob.isValid() == false) {
            throw CommonException("No blob found");
        }

        QByteArray oldData = oldBlob.rawData();
        QByteArray newData = newBlob.rawData();

        QStringList oldFileLines = QString(oldData).split('\n');
        QStringList newFileLines = QString(newData).split('\n');

        createTable(oldFileLines, newFileLines);
    }
    catch(const CommonException& e)
    {
        logText(LVL_ERROR, e.message());
    }
}

// --------------------- Index to Workdir Signature

DiffTableModel::DiffTableModel(GIT::Repository* repo, const GIT::DiffDelta& delta, QObject* parent) :
    AbstractTableModel("difftable", parent),
    _repo(repo)
{
    commonInit(delta);
    loadDiffToCurrent(delta);
}

void DiffTableModel::commonInit(const DiffDelta& delta)
{
    appendColumnHeader(CH_Old, "Old");
    appendColumnHeader(CH_New, "New");
    appendColumnHeader(CH_Tag, "Tag");
    appendColumnHeader(CH_Text, "Text");

    // consolidate delta lines
    for(const DiffHunk& hunk : delta.hunks()) {
        for(const DiffLine& diffLine : hunk.lines()) {
            if(diffLine.oldLineNumber() > 0) {
                if(_oldDiffLines.contains(diffLine.oldLineNumber()) == false) {
                    _oldDiffLines.insert(diffLine.oldLineNumber(), diffLine);
                }
                else {
                    logText(LVL_WARNING, "Old line already present");
                }
            }

            if(diffLine.newLineNumber() > 0) {
                if(_newDiffLines.contains(diffLine.newLineNumber()) == false) {
                    _newDiffLines.insert(diffLine.newLineNumber(), diffLine);
                }
                else {
                    logText(LVL_WARNING, "New line already present");
                }
            }
        }
    }

}

void DiffTableModel::loadDiffToCurrent(const DiffDelta& delta)
{
    try
    {
        Tree tree = _repo->head().tip().tree();
        TreeEntry oldEntry = tree.entries().findByPath(delta.oldFile().path());
        if(oldEntry.isValid() == false) {
            throw CommonException("No tree entry found");
        }

        Blob oldBlob = _repo->findBlob(oldEntry.targetObjectId());
        if(oldBlob.isValid() == false) {
            throw CommonException("No blob found");
        }

        QByteArray oldData = oldBlob.rawData();
        QStringList oldFileLines = QString(oldData).split('\n');

        QStringList newFileLines;
        QString localPath = PathUtil::combine(_repo->localPath(), delta.newFile().path());
        if(FileUtil::readAllLines(localPath, newFileLines) == false) {
            throw CommonException("File not found");
        }
        createTable(oldFileLines, newFileLines);
    }
    catch(const CommonException& e)
    {
        logText(LVL_ERROR, e.message());
    }
}

QModelIndexList DiffTableModel::nextDelta(int fromRow) const
{
    AbstractModelItem::List rootItems = DiffTableModel::rootItems();

    if(fromRow < 0) {
        fromRow = 0;
    }

    if(fromRow >= rootItems.count()) {
        return QModelIndexList();
    }

    // get off any delta we are currently on
    for(;fromRow < rootItems.count();fromRow++) {
        FileLineItem* item = static_cast<FileLineItem*>(rootItems.at(fromRow));
        if(item->isDelta() == false) {
            break;
        }
    }

    if(fromRow >= rootItems.count()) {
        return QModelIndexList();
    }

    for(;fromRow < rootItems.count();fromRow++) {
        FileLineItem* item = static_cast<FileLineItem*>(rootItems.at(fromRow));
        if(item->isDelta()) {
            QModelIndexList result;
            while(fromRow < rootItems.count()) {
                item = static_cast<FileLineItem*>(rootItems.at(fromRow));
                if(item->isDelta() == false) {
                    break;
                }
                QModelIndex index = createIndex(fromRow, 0, item);
                result.append(index);
                fromRow++;
            }
            return result;
        }
    }

    return QModelIndexList();
}

QModelIndexList DiffTableModel::previousDelta(int fromRow) const
{
    AbstractModelItem::List rootItems = DiffTableModel::rootItems();

    if(fromRow >= rootItems.count() - 1) {
        fromRow = rootItems.count() - 1;
    }

    if(fromRow < 0) {
        return QModelIndexList();
    }

    // get off any delta we are currently on
    for(;fromRow >= 0;fromRow--) {
        FileLineItem* item = static_cast<FileLineItem*>(rootItems.at(fromRow));
        if(item->isDelta() == false) {
            break;
        }
    }

    if(fromRow <= 0) {
        return QModelIndexList();
    }

    for(;fromRow >= 0;fromRow--) {
        FileLineItem* item = static_cast<FileLineItem*>(rootItems.at(fromRow));
        if(item->isDelta()) {
            QModelIndexList result;
            while(fromRow >= 0) {
                item = static_cast<FileLineItem*>(rootItems.at(fromRow));
                if(item->isDelta() == false) {
                    break;
                }
                QModelIndex index = createIndex(fromRow, 0, item);
                result.append(index);
                fromRow--;
            }
            std::sort(result.begin(), result.end(), [](const QModelIndex& a, const QModelIndex& b) { return a.row() < b.row(); } );
            return result;
        }
    }

    return QModelIndexList();
}

void DiffTableModel::createTable(const QStringList& oldFileLines, const QStringList& newFileLines)
{
    int oldLineNumber = 1;
    int newLineNumber = 1;
    while(newLineNumber < newFileLines.count() + 1 || oldLineNumber < oldFileLines.count() + 1) {
        if(_oldDiffLines.contains(oldLineNumber) == false && _newDiffLines.contains(newLineNumber) == false) {
            bool oldLinePresent = oldLineNumber < oldFileLines.count() + 1;
            bool newLinePresent = newLineNumber < newFileLines.count() + 1;
            QString line = newLinePresent
                           ? newFileLines.at(newLineNumber - 1)
                           : oldFileLines.at(oldLineNumber - 1);
            appendRootItem(new FileLineItem(line, oldLinePresent ? oldLineNumber : 0, newLinePresent ? newLineNumber : 0, QChar(), this));

            if(oldLinePresent) {
                oldLineNumber++;
            }

            if(newLinePresent) {
                newLineNumber++;
            }
            continue;
        }

        processLines(_oldDiffLines, oldLineNumber);
        processLines(_newDiffLines, newLineNumber);
    }
}

void DiffTableModel::processLines(QMap<int, GIT::DiffLine>& lines, int& lineNumber)
{
    if(lines.contains(lineNumber)) {
        DiffLineGroup group;
        while(lines.contains(lineNumber)) {
            DiffLine diffLine = lines.value(lineNumber);
            if(group.isValid() == false) {
                group = DiffLineGroup(rootItemCount(), diffLine.origin(), colorForOrigin(diffLine.origin()));
            }
            else if(group.origin() != diffLine.origin()) {
                _lineGroups.append(group);
                group = DiffLineGroup(rootItemCount(), diffLine.origin(), colorForOrigin(diffLine.origin()));
            }
            else {
                group.incrementCount();
            }
            appendRootItem(new FileLineItem(StringUtil::trimEnd(diffLine.content()), lineNumber, 0, diffLine.origin(), this));
            lineNumber++;
        }
        _lineGroups.append(group);
    }
}

QColor DiffTableModel::colorForOrigin(const QChar& origin)
{
    QColor result;
    switch(origin.toLatin1()) {
    case '+':
        result = Colors::lightgreen;
        break;
    case '-':
        result = Colors::lightsalmon;
        break;
    case 0:
        break;
    default:
        result = Colors::red;   // don't understand it
        break;
    }
    return result;
}

DiffTableModel::FileLineItem::FileLineItem(const QString& text, int oldLineNumber, int newLineNumber, const QChar& origin, DiffTableModel* model) :
    TableBaseItem(EntityMetadata(GitEntities::FileLine), model),
    _text(text), _oldLineNumber(oldLineNumber), _newLineNumber(newLineNumber), _origin(origin)
{
    _font.setFamily("Source Code Pro");
    _font.setPointSize(12);
    _font.setFixedPitch(true);
}

QVariant DiffTableModel::FileLineItem::data(const QModelIndex &index, int role) const
{
    QVariant result;
    switch(role) {
    case Qt::DisplayRole:
    {
        TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
        switch(header.type()) {

        case CH_Old:
            if(_oldLineNumber > 0) {
                result = _oldLineNumber;
            }
            break;
        case CH_New:
            if(_newLineNumber > 0) {
                result = _newLineNumber;
            }
            break;
        case CH_Tag:
            if(_origin.isNull() == false) {
                result = _origin;
            }
            break;
        case CH_Text:
            result = _text;
            break;

        default:
            break;
        }
        break;
    }
    case Qt::FontRole:
        result = _font;
        break;
    case Qt::BackgroundRole:
        if(_origin.isNull() == false) {
            result = static_cast<DiffTableModel*>(model())->colorForOrigin(_origin);
        }
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

