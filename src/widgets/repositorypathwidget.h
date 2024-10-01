#ifndef REPOSITORYPATHWIDGET_H
#define REPOSITORYPATHWIDGET_H
#include <git2qt.h>
#include <QWidget>

class RepositoryPathWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RepositoryPathWidget(QWidget* parent = nullptr);

    void setRepo(GIT::Repository* value);
    void setSubmodule(const GIT::Submodule& submodule);
    void setBranchName(const QString& branchName);

private:
    void createLayout();

    GIT::Repository* _repo;

    enum PartType
    {
        UnknownType,

        RepoNamePart,
        SubmodulePart,
        BranchPart,
    };

    class PathPart
    {
    public:
        PathPart() :
            _type(UnknownType) {}
        PathPart(PartType type, const QString& text) :
            _type(type), _text(text) {}

        PartType type() const { return _type; }
        QString text() const { return _text; }

    private:
        PartType _type;
        QString _text;
    };

    QList<PathPart> _pathParts;
};

#endif // REPOSITORYPATHWIDGET_H
