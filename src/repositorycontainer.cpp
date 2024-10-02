#include "repositorycontainer.h"
#include "repositorywidget.h"
#include "ui_repositorycontainer.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/pathutil.h>

#include <QMessageBox>
#include <QPushButton>

using namespace GIT;

RepositoryContainer::RepositoryContainer(const QString& path, QWidget *parent) :
    ComplexWidget("repocon", parent),
    ui(new Ui::RepositoryContainer)
{
    RepositoryContainer::setObjectName(RepositoryContainer::metaObject()->className());

    ui->setupUi(this);

    initializeBase();

    connect(ui->pathWidget, &RepositoryPathWidget::closeClicked, this, &RepositoryContainer::onPathWidgetCloseClicked);

    if(Repository::isRepository(path)) {
        _primaryRepo = new Repository(path);
        openRepository(_primaryRepo);
    }
}

RepositoryContainer::~RepositoryContainer()
{
    delete ui;
    qDeleteAll(_repos);
}

void RepositoryContainer::openRepository(GIT::Repository* repo)
{
    try
    {
        clearStackedWidget();

        if(_repoWidget != nullptr) {
            delete _repoWidget;
        }

        // Open repository
        _workingRepo = _repos.value(repo->localPath());
        if(_workingRepo == nullptr) {
            _workingRepo = new Repository(repo->localPath());
            _repos.insert(repo->localPath(), _workingRepo);
        }

        // Create the widget
        _repoWidget = new RepositoryWidget(_workingRepo, this);

        // Connect signals / slots
        connect(ui->pushPullFromRemote, &QPushButton::clicked, _repoWidget, &RepositoryWidget::pullFromRemote);
        connect(ui->pushPushToRemote, &QPushButton::clicked, _repoWidget, &RepositoryWidget::pushToRemote);
        connect(ui->pushStashChanges, &QPushButton::clicked, _repoWidget, &RepositoryWidget::stashChanges);
        connect(ui->pushPopStash, &QPushButton::clicked, _repoWidget, &RepositoryWidget::popStash);
        connect(ui->pushCreateBranch, &QPushButton::clicked, _repoWidget, &RepositoryWidget::beginCreateBranch);
        connect(ui->pushDebug, &QPushButton::clicked, _repoWidget, &RepositoryWidget::doDebugThing);

        connect(_repoWidget, &RepositoryWidget::validate, this, &RepositoryContainer::maybeEnableButtons);
        connect(_repoWidget, &RepositoryWidget::submoduleDoubleClicked, this, &RepositoryContainer::onSubmoduleDoubleClicked);

        ui->stackedWidget->addWidget(_repoWidget);
        ui->stackedWidget->setCurrentWidget(_repoWidget);

        initializePathWidget();
    }
    catch(CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message());
    }
}

void RepositoryContainer::clearStackedWidget()
{
    while(ui->stackedWidget->count() > 0) {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    }
}

void RepositoryContainer::setRepoWidget()
{
    clearStackedWidget();
    ui->stackedWidget->addWidget(_repoWidget)   ;
    ui->stackedWidget->setCurrentWidget(_repoWidget);
}

void RepositoryContainer::initializePathWidget()
{
    ui->pathWidget->setRepo(_primaryRepo);
    for(const QString& submodule : _submoduleStack) {
        ui->pathWidget->appendSubmodule(submodule);
    }
}

void RepositoryContainer::onSubmoduleDoubleClicked(const Submodule& submodule)
{
    try
    {
        QStringList stack = _submoduleStack;
        stack.prepend(_primaryRepo->localPath());
        stack.append(submodule.path());
        QString path = PathUtil::combine(stack);
        if(QDir(path).exists() == false) {
            throw CommonException(QString("Path: %1 does not exist").arg(path));
        }

        if(Repository::isRepository(path) == false) {
            throw CommonException(QString("Path %1 is not a repository").arg(path));
        }

        _submoduleStack.append(submodule.path());

        Repository* repo = new Repository(path);
        openRepository(repo);
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message());
    }
}

void RepositoryContainer::maybeEnableButtons()
{
    RepositoryStatus status = _primaryRepo->status();
    ui->pushStashChanges->setEnabled(status.entries().count() > 0);
    ui->pushCreateBranch->setEnabled(_primaryRepo->headCommit().isNull() == false);
    ui->pushPopStash->setEnabled(_repoWidget->isOnStash());
}

void RepositoryContainer::onPathWidgetCloseClicked()
{
    if(_workingRepo != nullptr && _workingRepo != _primaryRepo) {
        _submoduleStack.pop_back();
        openRepository(_primaryRepo);
    }
}
