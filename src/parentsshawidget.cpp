#include "parentsshawidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

#include <Kanoop/gui/stylesheets.h>

using namespace GIT;
using namespace TW;

ParentsShaWidget::ParentsShaWidget(const GIT::ObjectId::List& objectIds, QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QFont font = QWidget::font();
    font.setPointSize(8);
    font.setItalic(true);
    font.setUnderline(true);
    setFont(font);

    for(const ObjectId& objectId : objectIds) {
        ClickableLabel* label = new ClickableLabel(objectId, this);
        label->setFont(font);
        label->setStyleSheet(StyleSheets::foregroundColor(Qt::blue));
        layout->addWidget(label);
        connect(label, &ClickableLabel::clicked, this, &ParentsShaWidget::clicked);
    }
    setToolTip("Jump to commit");
    setLayout(layout);
}

ClickableLabel::ClickableLabel(const GIT::ObjectId& objectId, QWidget* parent) :
    QLabel(objectId.toString(6), parent),
    _objectId(objectId)
{
    setCursor(Qt::PointingHandCursor);
}

void TW::ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        emit clicked(_objectId);
    }
}
