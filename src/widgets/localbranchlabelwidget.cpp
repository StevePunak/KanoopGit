#include "localbranchlabelwidget.h"

#include <QHBoxLayout>
#include <settings.h>

#include <Kanoop/gui/widgets/label.h>

#include <Kanoop/utility/unicode.h>

using namespace GIT;

LocalBranchLabelWidget::LocalBranchLabelWidget(GIT::Repository* repo, const GIT::Reference& reference, QWidget *parent) :
    QWidget(parent),
    _reference(reference)
{
    QFont f = font();
    f.setPointSize(Settings::instance()->fontSize());
    setFont(f);

    _nameLabel = new Label(this);
    QString displayName = reference.friendlyName();
    int index = displayName.lastIndexOf('/');
    if(index >= 0 && index < displayName.length() - 1) {
        displayName = displayName.mid(index + 1);
    }
    _nameLabel->setText(displayName);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_nameLabel);

    _rightLabel = new Label(this);
    _rightLabel->setAlignment(Qt::AlignRight);
    _rightLabel->setVisible(false);
    layout->addWidget(_rightLabel);

    Branch localBranch = repo->findLocalBranch(reference.friendlyName());
    if(localBranch.isTracking()) {
        TrackingDetails trackingDetails = localBranch.trackingDetails();
        if(trackingDetails.isValid()) {
            QString labelText;
            if(trackingDetails.aheadBy() > 0) {
                labelText.append(QString("%1%2 ").arg(trackingDetails.aheadBy()).arg(Unicode::specialCharacter(Unicode::ArrowUp)));
            }
            if(trackingDetails.behindBy() > 0) {
                labelText.append(QString("%1%2 ").arg(trackingDetails.behindBy()).arg(Unicode::specialCharacter(Unicode::ArrowDown)));
            }

            if(labelText.isEmpty() == false) {
                _rightLabel->setText(labelText);
                _rightLabel->setVisible(true);
            }
        }
    }

    setLayout(layout);
}

void LocalBranchLabelWidget::setBold(bool value)
{
    QFont font = LocalBranchLabelWidget::font();
    font.setBold(value);
    _nameLabel->setFont(font);
    _rightLabel->setFont(font);
}

void LocalBranchLabelWidget::setSelected(bool value)
{
    _selected = value;
    _nameLabel->setBackgroundColor(_selected ? palette().color(QPalette::Highlight) : palette().color(QPalette::Window));
    _rightLabel->setBackgroundColor(_selected ? palette().color(QPalette::Highlight) : palette().color(QPalette::Window));
}
