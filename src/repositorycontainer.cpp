#include "repositorycontainer.h"
#include "ui_repositorycontainer.h"

RepositoryContainer::RepositoryContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepositoryContainer)
{
    ui->setupUi(this);
}

RepositoryContainer::~RepositoryContainer()
{
    delete ui;
}
