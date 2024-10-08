#include "toastwidgetcontainer.h"

#include <Kanoop/gui/utility/stylesheet.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include <Kanoop/geometry/point.h>
#include <Kanoop/gui/resources.h>
#include <Kanoop/gui/widgets/label.h>
#include <Kanoop/log.h>

namespace Colors = QColorConstants::Svg;

ToastWidgetContainer::ToastWidgetContainer(QWidget *parent) :
    QFrame(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    // setStyleSheet(StyleSheets::backgroundColor(Qt::blue));
    // setStyleSheet(StyleSheets::borderRadius(10));
}

void ToastWidgetContainer::displayToast(const QString& text, const QColor& color)
{
    ToastWidget* toast = new ToastWidget(text, color, this);

    QFontMetrics fm(font());
    QRect boundingRect = fm.boundingRect(0, 0, width(), 0, Qt::TextWordWrap, text);
    boundingRect.setHeight(boundingRect.height() + 48);     // close button
    boundingRect.setWidth(width());
    toast->setFixedSize(boundingRect.size());
    toast->show();

    performLayout();
}

void ToastWidgetContainer::performLayout()
{
    QList<ToastWidget*> toasts = findChildren<ToastWidget*>();

    // sort oldest to newest
    std::sort(toasts.begin(), toasts.end(), [](const ToastWidget* a, const ToastWidget* b) { return a->createTime() > b->createTime(); });

    int y = height() - 1;
    for(ToastWidget* toast : toasts) {
        y -= toast->height() + 1;
        Point pos(0, y);
        toast->move(pos.toPoint());
        Log::logText(LVL_DEBUG, QString("Moved toast to %1").arg(pos.toString()));
    }
}

ToastWidget::ToastWidget(const QString& text, const QColor& color, QWidget* parent) :
    QFrame(parent),
    _createTime(QDateTime::currentDateTimeUtc())
{
    _closeButton = new QToolButton(this);
    _closeButton->setIcon(Resources::getIcon(Resources::CloseButton));
    _closeButton->setFixedSize(16, 16);

    _label = new Label(text, this);
    _label->setWordWrap(true);
    _label->setAlignment(Qt::AlignCenter);
    _label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    _label->setBackgroundColor(Colors::lightcoral);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    titleLayout->addWidget(_closeButton);
    layout->addLayout(titleLayout);
    layout->addWidget(_label);

    setLayout(layout);

    StyleSheet<QFrame> ss;
    ss.setProperty(SP_Background, color);
    ss.setPropertyPixels(SP_BorderRadius, 10);

    setStyleSheet(ss.toString());
}
