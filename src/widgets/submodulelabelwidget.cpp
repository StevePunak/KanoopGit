#include "submodulelabelwidget.h"

#include <QHBoxLayout>
#include <settings.h>

#include <Kanoop/gui/widgets/label.h>
#include <Kanoop/gui/widgets/spinnerwidget.h>

#include <Kanoop/pathutil.h>

#include <Kanoop/gui/utility/unicode.h>

namespace Colors = QColorConstants::Svg;
using namespace GIT;

SubmoduleLabelWidget::SubmoduleLabelWidget(GIT::Repository* repo, const GIT::Submodule& submodule, QWidget *parent) :
    QWidget(parent),
    _submoduleRepo(nullptr),
    _submodule(submodule)
{
    if(submodule.isWorkdirInitialized()) {
Log::logText(LVL_DEBUG, QString("%1  %2  p1").arg(__FUNCTION__).arg(submodule.name()));
        QString path = PathUtil::combine(repo->localPath(), submodule.name());
        _submoduleRepo = new Repository(path);
        Commit headCommit = _submoduleRepo->headCommit();
        Commit otherCommit = _submoduleRepo->findCommit(_submodule.headCommitId());
        _commitsBehind = _submoduleRepo->commitDistance(headCommit, otherCommit);
Log::logText(LVL_DEBUG, QString("%1  %2  p2").arg(__FUNCTION__).arg(submodule.name()));
#if 0
        Commit::List allCommits = _submoduleRepo->allCommits();
Log::logText(LVL_DEBUG, QString("%1  %2  p3").arg(__FUNCTION__).arg(submodule.name()));
        int idx1 = allCommits.indexOf(headCommit);
        int idx2 = allCommits.indexOfObjectId(_submodule.headCommitId());
        _commitsBehind = idx2 - idx1;
Log::logText(LVL_DEBUG, QString("%1  %2  p4").arg(__FUNCTION__).arg(submodule.name()));
#endif
    }

    QFont f = font();
    f.setPointSize(Settings::instance()->fontSize());
    setFont(f);
    QFontMetrics fm(f);

    _nameLabel = new Label(this);
    _nameLabel->setText(submodule.name());

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_nameLabel);

    _rightLabel = new Label(this);
    _rightLabel->setAlignment(Qt::AlignRight);
    _rightLabel->setVisible(false);
    layout->addWidget(_rightLabel);

    _spinner = new SpinnerWidget(this);
    _spinner->setFixedSize(fm.height() - 1, fm.height() - 1);
    _spinner->setTextVisible(true);
    _spinner->setVisible(false);

    layout->addWidget(_spinner);
    if(_submodule.isWorkdirInitialized() == false) {
        setForegroundColor(Colors::darkorange);
    }
    else if(_commitsBehind != 0) {
        _rightLabel->setText(QString("%1 %2").arg(_commitsBehind).arg(Unicode::specialCharacter(Unicode::ArrowUp)));
        _rightLabel->setVisible(true);
        setForegroundColor(Colors::saddlebrown);
    }

    setLayout(layout);
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
    _nameLabel->setForegroundColor(color);
    _rightLabel->setForegroundColor(color);
}

void SubmoduleLabelWidget::setBackgroundColor(const QColor& color)
{
    _nameLabel->setBackgroundColor(color);
    _rightLabel->setBackgroundColor(color);
}

