#include "gitmainwindow.h"
#include "repositorywidget.h"
#include "ui_gitmainwindow.h"

#include <QFileDialog>
#include <kanoopgitsettings.h>

#include <QMessageBox>
#include <git2qt.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/stringutil.h>

#include "gitassets.h"
#include "gitroles.h"

#include <Kanoop/gui/resources.h>

using namespace GIT;

GitMainWindow::GitMainWindow(QWidget *parent) :
    MainWindowBase("gittool", parent),
    ui(new Ui::GitMainWindow)
{
    GitMainWindow::setObjectName(GitMainWindow::metaObject()->className());

    ui->setupUi(this);

    initializeBase();

    // Pushbuttons
    connect(ui->pushCloneRepo, &QPushButton::clicked, this, &GitMainWindow::onCloneRepoClicked);
    connect(ui->pushOpenRepo, &QPushButton::clicked, this, &GitMainWindow::onOpenRepoClicked);

    // Views
    connect(ui->tableRecentRepos, &RecentReposTableView::doubleClicked, this, &GitMainWindow::onRecentRepoDoubleClicked);

    // Other wiring
    connect(ui->tabWidgetRepos, &QTabWidget::tabCloseRequested, this, &GitMainWindow::onTabCloseRequested);
    connect(ui->tabWidgetRepos->tabBar(), &QTabBar::tabBarClicked, this, &GitMainWindow::onTabBarClicked);

    // Load models
    ui->tableRecentRepos->createModel(KanoopGitSettings::instance()->recentFiles());
    ui->stackedMain->setCurrentWidget(ui->pageRepos);
    ui->tabWidgetRepos->setMovable(true);

    // Other initialization
    ui->tabWidgetRepos->setTabIcon(0, Resources::getIcon(GitAssets::PlusGreen));

    openRecentRepos();
}

GitMainWindow::~GitMainWindow()
{
    delete ui;
}

void GitMainWindow::openRecentRepos()
{
    // open persisted repos
    RepositoryWidget* firstWidget = nullptr;
    for(int i = 0;i < KanoopGitSettings::instance()->openRepos().count();i++) {
        QString repoPath = KanoopGitSettings::instance()->openRepos().at(i);
        if(Repository::isRepository(repoPath)) {
            RepositoryWidget* widget = openRepository(repoPath);
            if(firstWidget == nullptr) {
                firstWidget = widget;
            }
        }
        else {
            logText(LVL_WARNING, QString("Removing invalid repo %1 from list").arg(repoPath));
            KanoopGitSettings::instance()->removeOpenRepo(repoPath);
        }
    }

    if(firstWidget != nullptr) {
        ui->tabWidgetRepos->setCurrentWidget(firstWidget);
    }
}

RepositoryWidget* GitMainWindow::openRepository(const QString& path)
{
    RepositoryWidget* repoWidget = nullptr;
    if(Repository::isRepository(path) == false) {
        return repoWidget;
    }

    repoWidget = new RepositoryWidget(path, this);
    QString title = QDir(path).dirName();
    int index = ui->tabWidgetRepos->addTab(repoWidget, title);

    QPushButton* closeButton = new QPushButton(Resources::getIcon(GitAssets::WindowClose), QString(), this);
    closeButton->setFixedSize(16, 16);
    connect(closeButton, &QPushButton::clicked, this, &GitMainWindow::onCloseTabClicked);
    ui->tabWidgetRepos->tabBar()->setTabButton(index, QTabBar::RightSide, closeButton);

    ui->tableRecentRepos->createModel(KanoopGitSettings::instance()->recentFiles());

    KanoopGitSettings::instance()->saveOpenRepo(path);

    return repoWidget;
}

void GitMainWindow::closeRepository(int tabIndex)
{
    RepositoryWidget* repoWidget = dynamic_cast<RepositoryWidget*>(ui->tabWidgetRepos->widget(tabIndex));
    if(repoWidget != nullptr) {
        KanoopGitSettings::instance()->removeOpenRepo(repoWidget->repository()->localPath());
    }
    ui->tabWidgetRepos->removeTab(tabIndex);
}

void GitMainWindow::onCloneRepoClicked()
{

}

void GitMainWindow::onOpenRepoClicked()
{
    try
    {
        QString dirName = QFileDialog::getExistingDirectory(this, "Open Existing Repository", KanoopGitSettings::instance()->lastDirectory("repo"));
        if(dirName.isEmpty() == false) {
            if(Repository::isRepository(dirName) == false) {
                throw CommonException("No repository found at give path");
            }
            // save settings
            KanoopGitSettings::instance()->pushRecentFile(dirName);
            KanoopGitSettings::instance()->saveLastDirectory("repo", dirName);

            openRepository(dirName);
        }
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message(), QMessageBox::Ok);
    }
}

void GitMainWindow::onCloseTabClicked()
{
    QWidget* button = static_cast<QWidget*>(sender());
    QTabBar* tabBar = dynamic_cast<QTabBar*>(button->parent());
    if(tabBar != nullptr) {
        int index = 0;
        for(;index < tabBar->count() && tabBar->tabButton(index, QTabBar::RightSide) != button;index++);
        if(index < tabBar->count()) {
            closeRepository(index);
        }
    }
}

void GitMainWindow::onRecentRepoDoubleClicked(const QModelIndex& index)
{
    QString path = index.data(RepoPathRole).toString();
    if(Repository::isRepository(path)) {
        openRepository(path);
    }
}

void GitMainWindow::onTabCloseRequested(int index)
{
    logText(LVL_DEBUG, QString("%1 %2").arg(__FUNCTION__).arg(index));
}

void GitMainWindow::onTabBarClicked(int index)
{
    bool visible = ui->tabWidgetRepos->tabBar()->isTabVisible(0);
    logText(LVL_DEBUG, QString("%1 %2  visible: %3").arg(__FUNCTION__).arg(index).arg(StringUtil::toString(visible)));
}
