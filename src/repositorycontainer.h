#ifndef REPOSITORYCONTAINER_H
#define REPOSITORYCONTAINER_H

#include <Kanoop/gui/complexwidget.h>
#include <git2qt.h>

namespace Ui {
class RepositoryContainer;
}

class RepositoryContainer : public ComplexWidget
{
    Q_OBJECT

public:
    explicit RepositoryContainer(const QString& path, QWidget *parent = nullptr);
    ~RepositoryContainer();

private:
    Ui::RepositoryContainer *ui;
    GIT::Repository* _repo = nullptr;
    GIT::Repository* _submoduleRepo = nullptr;
};

#endif // REPOSITORYCONTAINER_H
