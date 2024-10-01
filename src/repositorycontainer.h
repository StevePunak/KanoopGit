#ifndef REPOSITORYCONTAINER_H
#define REPOSITORYCONTAINER_H

#include <QWidget>

namespace Ui {
class RepositoryContainer;
}

class RepositoryContainer : public QWidget
{
    Q_OBJECT

public:
    explicit RepositoryContainer(QWidget *parent = nullptr);
    ~RepositoryContainer();

private:
    Ui::RepositoryContainer *ui;
};

#endif // REPOSITORYCONTAINER_H
