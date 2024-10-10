#include "submodulelabelwidget.h"

#include <QHBoxLayout>
#include <gitassets.h>
#include <settings.h>

#include <Kanoop/gui/widgets/label.h>
#include <Kanoop/gui/widgets/spinnerwidget.h>

#include <Kanoop/pathutil.h>

#include <Kanoop/gui/utility/unicode.h>

#include <Kanoop/gui/resources.h>

namespace Colors = QColorConstants::Svg;
using namespace GIT;

SubmoduleLabelWidget::SubmoduleLabelWidget(GIT::Repository* repo, const GIT::Submodule& submodule, QWidget *parent) :
    QWidget(parent),
    _submoduleRepo(nullptr),
    _submodule(submodule)
{
    int commitsBehind = 0;
    bool workDirDirty = false;
    Submodule::SubmoduleStatus status = _submodule.status(/*Submodule::IgnoreDirty*/);
    if(Submodule::isWorkdirInitialized(status)) {
        // workdir is initialized
        QString path = PathUtil::combine(repo->localPath(), submodule.name());
        if(Repository::isRepository(path)) {
            _submoduleRepo = new Repository(path);
            Commit headCommit = _submoduleRepo->headCommit();
            Commit otherCommit = _submoduleRepo->findCommit(_submodule.headCommitId());
            if(headCommit.isValid() && otherCommit.isValid()) {
                commitsBehind = _submoduleRepo->commitDistance(headCommit, otherCommit);
            }
            if(status & Submodule::IndexAdded) {
                // freshly added... not in HEAD of super-repo
                _indexAddedButNotInHead = true;
            }

            if(commitsBehind == 0 && Submodule::isWorkDirDirty(status)) {
                workDirDirty = true;
            }
        }
    }

    QFont f = font();
    f.setPointSize(Settings::instance()->fontSize());
    setFont(f);
    QFontMetrics fm(f);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSize littleLabelSize(fm.height() - 1, fm.height() - 1);
    QSize iconLabelSize = littleLabelSize.shrunkBy(QMargins(2,2,2,2));

    _iconLabel = new Label(this);
    _iconLabel->setFixedSize(iconLabelSize);
    _iconLabel->setScaledContents(true);
    if(commitsBehind != 0) {
        _iconLabel->setPixmap(Resources::getPixmap(GitAssets::OrangeRefresh));
    }
    else if(_indexAddedButNotInHead) {
        _iconLabel->setPixmap(Resources::getPixmap(GitAssets::PlusGreen));
    }
    else if(workDirDirty) {
        _iconLabel->setPixmap(Resources::getPixmap(GitAssets::OrangeRefresh));
    }
    else {
        _iconLabel->setPixmap(Resources::getPixmap(GitAssets::CheckGreen));
    }
    layout->addWidget(_iconLabel);
    layout->addSpacing(3);

    _nameLabel = new Label(this);
    _nameLabel->setText(submodule.name());
    layout->addWidget(_nameLabel);

    _rightLabel = new Label(this);
    _rightLabel->setAlignment(Qt::AlignRight);
    _rightLabel->setVisible(false);
    layout->addWidget(_rightLabel);

    _spinner = new SpinnerWidget(this);
    _spinner->setFixedSize(littleLabelSize);
    _spinner->setTextVisible(true);
    _spinner->setVisible(false);

    layout->addWidget(_spinner);
    if(_submodule.isWorkdirInitialized() == false) {
        setForegroundColor(Colors::darkred);
    }
    else if(commitsBehind != 0) {
        _rightLabel->setText(QString("%1 %2").arg(commitsBehind).arg(Unicode::specialCharacter(Unicode::ArrowUp)));
        _rightLabel->setVisible(true);
        setForegroundColor(Colors::saddlebrown);
    }
    else if(workDirDirty) {
        setForegroundColor(Colors::saddlebrown);
    }

    setLayout(layout);

    setToolTip(submodule.statusDebugString(status));
}

SubmoduleLabelWidget::~SubmoduleLabelWidget()
{
    if(_submoduleRepo != nullptr) {
        delete _submoduleRepo;
    }
}

bool SubmoduleLabelWidget::isSpinning() const
{
    return _spinner->isVisible() && _spinner->isSpinning();
}

int SubmoduleLabelWidget::spinnerValue() const
{
    return _spinner->value();
}

void SubmoduleLabelWidget::setSpinning(bool value)
{
    _spinner->setSpinning(value);
}

void SubmoduleLabelWidget::setSelected(bool value)
{
    _selected = value;
    QColor backColor = _selected ? palette().color(QPalette::Highlight) : palette().color(QPalette::Window);
    setBackgroundColor(backColor);
}

void SubmoduleLabelWidget::setSpinnerVisible(bool value)
{
    _spinner->setVisible(value);
}

void SubmoduleLabelWidget::hideSpinnerIn(const TimeSpan& delay)
{
    QTimer::singleShot(delay.totalMilliseconds(), _spinner, &SpinnerWidget::hide);
}

void SubmoduleLabelWidget::setSpinnerValue(int value)
{
    _spinner->setValue(value);
}

void SubmoduleLabelWidget::setForegroundColor(const QColor& color)
{
    _iconLabel->setForegroundColor(color);
    _nameLabel->setForegroundColor(color);
    _rightLabel->setForegroundColor(color);
}

void SubmoduleLabelWidget::setBackgroundColor(const QColor& color)
{
    _iconLabel->setBackgroundColor(color);
    _nameLabel->setBackgroundColor(color);
    _rightLabel->setBackgroundColor(color);
}

