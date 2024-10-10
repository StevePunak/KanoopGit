#include "repositorypathwidget.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include <Kanoop/gui/widgets/buttonlabel.h>
#include <Kanoop/gui/widgets/label.h>

#include <Kanoop/gui/resources.h>

using namespace GIT;

RepositoryPathWidget::RepositoryPathWidget(QWidget* parent) :
    QWidget(parent)
{
}

void RepositoryPathWidget::setRepo(GIT::Repository* value)
{
    _repo = value;

    QFileInfo fileInfo(_repo->localPath());

    _pathParts.clear();
    _pathParts.append(PathPart(RepoNamePart, fileInfo.baseName()));

    createLayout();
}

void RepositoryPathWidget::appendSubmodule(const QString& submodule)
{
    _pathParts.append(PathPart(SubmodulePart, submodule));
    createLayout();
}

void RepositoryPathWidget::createLayout()
{
    const int SmallTextSize = 7;

    if(layout() != nullptr) {
        while(layout()->count() > 0) {
            QLayoutItem* item = layout()->takeAt(0);
            delete item->widget();
            delete item;
        }
        delete layout();
    }

    QGridLayout* newLayout = new QGridLayout(this);
    for(int col = 0;col < _pathParts.count();col++) {
        PathPart pathPart = _pathParts[col];
        switch(pathPart.type()) {
        case RepoNamePart:
        {
            Label* titleLabel = new Label("Repository", this);
            titleLabel->setFontPointSize(SmallTextSize);
            Label* nameLabel = new Label(pathPart.text(), this);
            newLayout->addWidget(titleLabel, 0, col);
            newLayout->addWidget(nameLabel, 1, col);
            break;
        }
        case SubmodulePart:
        {
            Label* titleLabel = new Label("Submodule", this);
            titleLabel->setFontPointSize(SmallTextSize);
            ButtonLabel* nameLabel = new ButtonLabel(pathPart.text(), this);
            connect(nameLabel, &ButtonLabel::clicked, this, &RepositoryPathWidget::closeClicked);
            nameLabel->setButtonAlignment(Qt::AlignLeft);
            nameLabel->setIcon(Resources::getIcon(Resources::CloseButton));
            newLayout->addWidget(titleLabel, 0, col);
            newLayout->addWidget(nameLabel, 1, col);
            break;
        }
        default:
            break;
        }
    }
    setLayout(newLayout);

}
