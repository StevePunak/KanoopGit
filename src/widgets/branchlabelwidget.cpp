#include "branchlabelwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <gitassets.h>

#include <Kanoop/gui/resources.h>

#include <Kanoop/geometry/size.h>

using namespace GIT;

BranchLabelWidget::BranchLabelWidget(const GIT::Reference::List &references, QWidget *parent) :
    QWidget(parent),
    _references(references)
{
    BranchLabelWidget::setObjectName(BranchLabelWidget::metaObject()->className());

    _combo = new QComboBox(this);

    int widgetHeight = _combo->height() - 2;
    QPixmap cloudPixmap = Resources::getPixmap(GitAssets::Cloud);
    QPixmap computerPixmap = Resources::getPixmap(GitAssets::Computer);
    Size pixmapSize(widgetHeight / 2, widgetHeight / 2);
    cloudPixmap = cloudPixmap.scaled(pixmapSize.toSize());
    computerPixmap = computerPixmap.scaled(pixmapSize.toSize());

    for(const Reference& reference : _references) {
        QIcon icon = reference.isRemote() ? QIcon(cloudPixmap) : QIcon(computerPixmap);
        _combo->addItem(icon, reference.friendlyName(), reference.toVariant());
    }


    QLabel* label = new QLabel(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(_combo);
    layout->addWidget(label);
}
