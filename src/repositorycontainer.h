#ifndef REPOSITORYCONTAINER_H
#define REPOSITORYCONTAINER_H

#include "repoconfig.h"

#include <Kanoop/gui/complexwidget.h>
#include <git2qt.h>

namespace Ui {
class RepositoryContainer;
}

class RepositoryWidget;
class RepositoryContainer : public ComplexWidget
{
    Q_OBJECT

public:
    explicit RepositoryContainer(const QString& path, QWidget *parent = nullptr);
    ~RepositoryContainer();

    GIT::Repository* primaryRepo() const { return _primaryRepo; }
    GIT::Repository* workingRepo() const { return _workingRepo; }

private:
    void openRepository(GIT::Repository* repo);
    void openSubmoduleFromStack();
    void openPersistedSubmodules();
    GIT::Repository* openSubmodule(const GIT::Submodule& submodule);
    void clearStackedWidget();
    void setRepoWidget();
    void initializePathWidget();
    void persistSubmodulePath() const;

    Ui::RepositoryContainer *ui;

    GIT::Repository* _primaryRepo = nullptr;
    GIT::Repository* _workingRepo = nullptr;
    RepositoryWidget* _repoWidget = nullptr;

    QStringList _submoduleStack;
    QMap<QString, GIT::Repository*> _repos;

private slots:
    void onOpenSubmoduleClicked(const GIT::Submodule& submodule);
    void maybeEnableButtons();
    void onPathWidgetCloseClicked(const QString& text);
};

#endif // REPOSITORYCONTAINER_H
