#include "shawidget.h"

#include "gitassets.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QToolTip>

#include <Kanoop/gui/resources.h>

ShaWidget::ShaWidget(const GIT::ObjectId& objectId, QWidget *parent) :
    QWidget(parent),
    _objectId(objectId)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    _shaLabel = new QLabel(objectId.toString(6), this);
    QFont font = QWidget::font();
    font.setPointSize(8);
    font.setItalic(true);
    _shaLabel->setFont(font);

    _copyButton = new QToolButton(this);
    _copyButton->setIcon(Resources::getIcon(GitAssets::Copy));
    _copyButton->setToolTip("Click to copy SHA");

    QFontMetrics fm(font);
    _copyButton->setFixedSize(fm.height(), fm.height());

    layout->addWidget(_shaLabel);
    layout->addWidget(_copyButton);

    setLayout(layout);

    connect(_copyButton, &QToolButton::clicked, this, &ShaWidget::onCopyClicked);
}

void ShaWidget::onCopyClicked()
{
    QGuiApplication::clipboard()->setText(_objectId.toString());
    QToolTip::showText(QCursor::pos(), "Copied", this);
}
