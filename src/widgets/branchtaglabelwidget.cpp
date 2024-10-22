#include "branchtaglabelwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <gitassets.h>
#include <settings.h>

#include <Kanoop/gui/resources.h>

#include <Kanoop/geometry/point.h>
#include <Kanoop/geometry/size.h>

#include <Kanoop/gui/widgets/combobox.h>
#include <Kanoop/gui/widgets/iconlabel.h>

using namespace GIT;

BranchTagLabelWidget::BranchTagLabelWidget(Repository* repo, const ReferenceList& references, const AnnotatedTag::List& annotatedTags, const LightweightTag::List& lightweightTags, QWidget *parent) :
    QWidget(parent),
    _repo(repo),
    _references(references),
    _annotatedTags(annotatedTags),
    _lightweightTags(lightweightTags)
{
    BranchTagLabelWidget::setObjectName(BranchTagLabelWidget::metaObject()->className());

    QFont f = font();
    f.setPointSize(Settings::instance()->fontSize());
    setFont(f);

    QPixmap labelPixmap = _noLinePixmap;
    ObjectId headCommitId = repo->headCommit().objectId();
    Branch currentBranch = repo->currentBranch();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    if(references.count() + _annotatedTags.count() + _lightweightTags.count() > 1) {
        _combo = new ComboBox(this);
        createPixmaps();

        int setIndex = 0;
        int row = 0;
        for(const Reference& reference : _references) {
            QIcon icon = reference.isRemote() ? QIcon(_cloudPixmap) : QIcon(_computerPixmap);
            _combo->addItem(icon, reference.friendlyName(), reference.toVariant());
            if(currentBranch.friendlyName() == reference.friendlyName()) {
                _combo->setBold(true);
                _combo->setRowBold(row, true);
                setIndex = row;
            }
            else {
                _combo->setRowBold(row, false);
            }
            if(reference.targetObjectId() == headCommitId) {
                labelPixmap = _linePixmap;
            }
            row++;
        }
        for(const AnnotatedTag& tag : _annotatedTags) {
            _combo->addItem(QIcon(_tagPixmap), tag.shortName());
        }
        for(const LightweightTag& tag : _lightweightTags) {
            _combo->addItem(QIcon(_tagPixmap), tag.shortName());
        }

        _combo->setCurrentIndex(setIndex);
        layout->addWidget(_combo);
    }
    else if(references.count() > 0){
        Reference reference = _references.at(0);
        _nameLabel = new IconLabel(reference.friendlyName(), this);
        createPixmaps();
        if(reference.targetObjectId() == headCommitId) {
            labelPixmap = _linePixmap;
        }
        layout->addWidget(_nameLabel);
    }
    else if(annotatedTags.count() > 0 || lightweightTags.count() > 0) {
        QString text = annotatedTags.count() > 0 ? annotatedTags.at(0).shortName() : lightweightTags.at(0).shortName();
        _nameLabel = new IconLabel(text, Resources::getIcon(GitAssets::Tag), this);
        createPixmaps();
        layout->addWidget(_nameLabel);
    }

    _lineLabel = new QLabel(this);
    _lineLabel->setPixmap(labelPixmap);
    _lineLabel->setFixedWidth(20);
    layout->addWidget(_lineLabel);
}

QString BranchTagLabelWidget::currentReferenceFriendlyName() const
{
    QString result;
    if(_combo != nullptr) {
        Reference reference = Reference::fromVariant(_combo->currentData());
        if(reference.isNull() == false) {
            result = reference.friendlyName();
        }
    }
    else {
        result = _nameLabel->text();
    }
    return result;
}

QString BranchTagLabelWidget::firstReferenceName() const
{
    QString result;
    if(_references.count() > 0) {
        result = _references.at(0).friendlyName();
    }
    return result;
}

void BranchTagLabelWidget::createPixmaps()
{
    double widgetHeight = labelWidget()->height() - 2;
    _cloudPixmap = Resources::getPixmap(GitAssets::Cloud);
    _computerPixmap = Resources::getPixmap(GitAssets::Computer);
    _tagPixmap = Resources::getPixmap(GitAssets::Tag);
    Size pixmapSize(widgetHeight / 2, widgetHeight / 2);
    _cloudPixmap = _cloudPixmap.scaled(pixmapSize.toSize());
    _computerPixmap = _computerPixmap.scaled(pixmapSize.toSize());

    _linePixmap = QPixmap(256, widgetHeight);     _linePixmap.fill(Qt::transparent);
    _noLinePixmap = QPixmap(256, widgetHeight);   _noLinePixmap.fill(Qt::transparent);

    QPainter painter(&_linePixmap);
    GitGraphPalette graphPalette;
    painter.setPen(QPen(graphPalette.headCommitLineColor(), graphPalette.headCommitLineWidth()));
    Point p1(0, labelWidget()->height() / 2);
    Point p2(_linePixmap.width(), labelWidget()->height() / 2);
    painter.drawLine(p1, p2);
}

QWidget* BranchTagLabelWidget::labelWidget() const
{
    return _combo != nullptr ? (QWidget*)_combo : (QWidget*)_nameLabel;
}


