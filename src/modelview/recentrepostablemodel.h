#ifndef RECENTREPOSTABLEMODEL_H
#define RECENTREPOSTABLEMODEL_H
#include <Kanoop/gui/abstracttablemodel.h>
#include <Kanoop/gui/abstractmodelitem.h>
#include "kanoopgittypes.h"
#include "gitentities.h"

class RecentReposTableModel : public AbstractTableModel
{
    Q_OBJECT
public:
    RecentReposTableModel(const QStringList& repoPaths, QObject* parent = nullptr);

private:
    class RepoPathItem : public AbstractModelItem
    {
    public:
        explicit RepoPathItem(const QString& fullPath, const QString& displayPath, RecentReposTableModel* model) :
            AbstractModelItem(EntityMetadata(GitEntities::Repository), model),
            _fullPath(fullPath), _displayPath(displayPath) {}

        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        QString _fullPath;
        QString _displayPath;
    };

};

#endif // RECENTREPOSTABLEMODEL_H
