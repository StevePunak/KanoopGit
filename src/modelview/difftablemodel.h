#ifndef DIFFTABLEMODEL_H
#define DIFFTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include <QFont>
#include <git2qt.h>

class DiffTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    DiffTableModel(GIT::Repository* repo, const GIT::Tree& oldTree, const GIT::Tree& newTree, const GIT::DiffDelta& delta, QObject* parent = nullptr);
    DiffTableModel(GIT::Repository* repo, const GIT::DiffDelta& delta, QObject* parent = nullptr);

    QModelIndexList nextDelta(int fromRow) const;
    QModelIndexList previousDelta(int fromRow) const;

private:
    void commonInit(const GIT::DiffDelta& delta);
    void createTable(const QStringList& oldFileLines, const QStringList& newFileLines);

    GIT::Repository* _repo;
    QMap<int, GIT::DiffLine> _oldDiffLines;
    QMap<int, GIT::DiffLine> _newDiffLines;

    class TableBaseItem : public AbstractModelItem
    {
    public:
        explicit TableBaseItem(const EntityMetadata& metadata, DiffTableModel* model) :
            AbstractModelItem(metadata, model) {}

    protected:
        GIT::Repository* repo() const { return static_cast<DiffTableModel*>(model())->_repo; }
    };

    class FileLineItem : public TableBaseItem
    {
    public:
        explicit FileLineItem(const QString& text, int oldLineNumber, int newLineNumber, const QChar& origin, DiffTableModel* model);

        virtual QVariant data(const QModelIndex &index, int role) const override;
        bool isDelta() const { return _origin.isNull() == false; }
        int oldLineNumber() const { return _oldLineNumber; }
        int newLineNumber() const { return _newLineNumber; }

    private:
        QString _text;
        int _oldLineNumber;
        int _newLineNumber;
        QChar _origin;

        QFont _font;
    };
};

#endif // DIFFTABLEMODEL_H
