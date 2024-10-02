#include "repositorycontainer.h"
#include "repositorywidget.h"
#include "ui_repositorycontainer.h"

using namespace GIT;

RepositoryContainer::RepositoryContainer(const QString& path, QWidget *parent) :
    ComplexWidget("repocon", parent),
    ui(new Ui::RepositoryContainer)
{
    RepositoryContainer::setObjectName(RepositoryContainer::metaObject()->className());

    ui->setupUi(this);

    initializeBase();

    _repo = new Repository(path);

    RepositoryWidget* repoWidget = new RepositoryWidget(_repo, this);
    ui->stackedWidget->addWidget(repoWidget);
    ui->stackedWidget->setCurrentWidget(repoWidget);
}

RepositoryContainer::~RepositoryContainer()
{
    delete ui;
}
