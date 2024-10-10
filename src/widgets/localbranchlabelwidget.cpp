#include "localbranchlabelwidget.h"

#include <QHBoxLayout>
#include <settings.h>

#include <Kanoop/gui/widgets/label.h>

#include <Kanoop/gui/utility/unicode.h>

using namespace GIT;

LocalBranchLabelWidget::LocalBranchLabelWidget(GIT::Repository* repo, const GIT::Reference& reference, QWidget *parent) :
    QWidget(parent)
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
    Branch remoteBranch = localBranch.trackedBranch();
    if(remoteBranch.isNull() == false) {
        Commit commitA = localBranch.tip();
        Commit commitB = remoteBranch.tip();

        if(commitA.isValid() && commitB.isValid()) {
            int distance = repo->commitDistance(commitA, commitB);
            if(distance > 0) {
                _rightLabel->setText(QString("%1 %2").arg(distance).arg(Unicode::specialCharacter(Unicode::ArrowUp)));
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
