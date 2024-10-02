#include "gitmainwindow.h"
#include "repositorywidget.h"
#include "ui_gitmainwindow.h"

#include <QFileDialog>
#include <settings.h>

#include <QMessageBox>
#include <git2qt.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/stringutil.h>

#include "gitassets.h"
#include "kanoopgittypes.h"
#include "repositorycontainer.h"

#include <Kanoop/gui/resources.h>

#include <dialogs/clonerepodialog.h>
#include <dialogs/preferencesdialog.h>

using namespace GIT;

GitMainWindow::GitMainWindow(QWidget *parent) :
    MainWindowBase("gittool", parent),
    ui(new Ui::GitMainWindow),
    _progressCallback(this)
{
    GitMainWindow::setObjectName(GitMainWindow::metaObject()->className());

    ui->setupUi(this);

    initializeBase();

    // Pushbuttons
    connect(ui->pushCloneRepo, &QPushButton::clicked, this, &GitMainWindow::onCloneRepoClicked);
    connect(ui->pushOpenRepo, &QPushButton::clicked, this, &GitMainWindow::onOpenRepoClicked);

    // Views
    connect(ui->tableRecentRepos, &RecentReposTableView::doubleClicked, this, &GitMainWindow::onRecentRepoDoubleClicked);

    // Actions
    connect(ui->actionCloneRepository, &QAction::triggered, this, &GitMainWindow::onCloneRepoClicked);
    connect(ui->actionOpenRepository, &QAction::triggered, this, &GitMainWindow::onOpenRepoClicked);
    connect(ui->actionPreferences, &QAction::triggered, this, &GitMainWindow::onPreferencesClicked);

    // Other wiring
    connect(ui->tabWidgetRepos, &QTabWidget::tabCloseRequested, this, &GitMainWindow::onTabCloseRequested);
    connect(ui->tabWidgetRepos->tabBar(), &QTabBar::tabBarClicked, this, &GitMainWindow::onTabBarClicked);
    connect(&_progressCallback, &CloneProgressCallback::progress, this, &GitMainWindow::onCloneProgress);

    // Load models
    ui->tableRecentRepos->createModel(Settings::instance()->recentFiles());
    ui->stackedMain->setCurrentWidget(ui->pageRepos);
    ui->tabWidgetRepos->setMovable(true);

    // Other initialization
    ui->tabWidgetRepos->setTabIcon(0, Resources::getIcon(GitAssets::PlusGreen));

    ui->progressCloneProgress->setVisible(false);

    openRecentRepos();
}

GitMainWindow::~GitMainWindow()
{
    delete ui;
}

void GitMainWindow::openRecentRepos()
{
    // open persisted repos
    RepositoryContainer* firstWidget = nullptr;
    for(int i = 0;i < Settings::instance()->openRepos().count();i++) {
        QString repoPath = Settings::instance()->openRepos().at(i);
        if(Repository::isRepository(repoPath)) {
            RepositoryContainer* widget = openRepository(repoPath);
            if(firstWidget == nullptr) {
                firstWidget = widget;
            }
        }
        else {
            logText(LVL_WARNING, QString("Removing invalid repo %1 from list").arg(repoPath));
            Settings::instance()->removeOpenRepo(repoPath);
        }
    }

    if(firstWidget != nullptr) {
        ui->tabWidgetRepos->setCurrentWidget(firstWidget);
    }
}

RepositoryContainer* GitMainWindow::openRepository(const QString& path)
{
    RepositoryContainer* repoWidget = nullptr;
    if(Repository::isRepository(path) == false) {
        return repoWidget;
    }

    repoWidget = new RepositoryContainer(path, this);
    QString title = QDir(path).dirName();
    int index = ui->tabWidgetRepos->addTab(repoWidget, title);

    QPushButton* closeButton = new QPushButton(Resources::getIcon(GitAssets::WindowClose), QString(), this);
    closeButton->setFixedSize(16, 16);
    connect(closeButton, &QPushButton::clicked, this, &GitMainWindow::onCloseTabClicked);
    ui->tabWidgetRepos->tabBar()->setTabButton(index, QTabBar::RightSide, closeButton);

    ui->tableRecentRepos->createModel(Settings::instance()->recentFiles());

    Settings::instance()->saveOpenRepo(path);

    return repoWidget;
}

void GitMainWindow::closeRepository(int tabIndex)
{
    RepositoryWidget* repoWidget = dynamic_cast<RepositoryWidget*>(ui->tabWidgetRepos->widget(tabIndex));
    if(repoWidget != nullptr) {
        Settings::instance()->removeOpenRepo(repoWidget->repository()->localPath());
    }
    ui->tabWidgetRepos->removeTab(tabIndex);
}

void GitMainWindow::onCloneRepoClicked()
{
    try
    {
        CloneRepoDialog dlg(this);
        if(dlg.exec() == QDialog::Accepted) {
            QString localPath = dlg.localPath();
            QString url = dlg.url();
            GitCredentialResolver credentialResolver(dlg.credentials());

            ui->progressCloneProgress->setVisible(true);

            Repository* repo = Commands::clone(url, localPath, &credentialResolver, &_progressCallback);
            if(repo == nullptr) {
                throw CommonException(Commands::lastErrorText());
            }
            delete repo;

            RepositoryContainer* widget = openRepository(localPath);
            if(widget != nullptr) {
                ui->tabWidgetRepos->setCurrentWidget(widget);
            }

            ui->progressCloneProgress->setVisible(false);
        }
    }
    catch(const CommonException& e)
    {
        QMessageBox::warning(this, "Error", e.message(), QMessageBox::Ok);
    }
}

void GitMainWindow::onOpenRepoClicked()
{
    try
    {
        QString dirName = QFileDialog::getExistingDirectory(this, "Open Existing Repository", Settings::instance()->lastDirectory(RepoDirectory));
        if(dirName.isEmpty() == false) {
            if(Repository::isRepository(dirName) == false) {
                throw CommonException("No repository found at give path");
            }
            // save settings
            Settings::instance()->pushRecentFile(dirName);
            Settings::instance()->saveLastDirectory(RepoDirectory, dirName);

            RepositoryContainer* widget = openRepository(dirName);
            if(widget != nullptr) {
                ui->tabWidgetRepos->setCurrentWidget(widget);
            }
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
        RepositoryContainer* widget = openRepository(path);
        if(widget != nullptr) {
            ui->tabWidgetRepos->setCurrentWidget(widget);
        }
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

void GitMainWindow::onPreferencesClicked()
{
    PreferencesDialog dlg(this);
    dlg.exec();
}

void GitMainWindow::onCloneProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    Q_UNUSED(receivedBytes)
    ui->progressCloneProgress->setMaximum(totalObjects);
    ui->progressCloneProgress->setValue(receivedObjects);
}
