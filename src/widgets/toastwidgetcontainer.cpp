#include "toastwidgetcontainer.h"

#include <Kanoop/gui/utility/stylesheet.h>

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QToolButton>

#include <Kanoop/geometry/point.h>
#include <Kanoop/gui/resources.h>
#include <Kanoop/gui/widgets/label.h>
#include <Kanoop/log.h>

namespace Colors = QColorConstants::Svg;

ToastWidgetContainer::ToastWidgetContainer(QWidget *parent) :
    QFrame(parent),
    _autoCloseTime(TimeSpan::fromSeconds(5))
{
    // setAttribute(Qt::WA_TranslucentBackground);
    // setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void ToastWidgetContainer::displayToast(const QString& text, const QColor& color)
{
    ToastWidget* toast = new ToastWidget(text, color, _autoCloseTime, this);
    connect(toast, &ToastWidget::complete, this, &ToastWidgetContainer::closeToast);

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

void ToastWidgetContainer::closeToast()
{
    Log::logText(LVL_DEBUG, "Close toast");
    ToastWidget* widget = dynamic_cast<ToastWidget*>(sender());
    if(widget != nullptr) {
        delete widget;
        performLayout();
    }
}

ToastWidget::ToastWidget(const QString& text, const QColor& color, const TimeSpan& closeTime, QWidget* parent) :
    QFrame(parent),
    _color(color),
    _createTime(QDateTime::currentDateTimeUtc()),
    _opacity(1.0)
{
    // setAttribute(Qt::WA_TranslucentBackground);
    // setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    _closeButton = new QToolButton(this);
    _closeButton->setIcon(Resources::getIcon(Resources::CloseButton));
    _closeButton->setFixedSize(16, 16);
    _closeButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    connect(_closeButton, &QToolButton::clicked, this, &ToastWidget::complete);
    connect(_closeButton, &QToolButton::clicked, this, &ToastWidget::onClick);

    _label = new Label(text, this);
    _label->setWordWrap(true);
    _label->setAlignment(Qt::AlignCenter);
    _label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    _opacityEffect = new QGraphicsOpacityEffect(this);
    _opacityEffect->setOpacity(_opacity);
    setGraphicsEffect(_opacityEffect);

    StyleSheet<QFrame> ss;
    ss.setProperty(SP_Background, color);
    ss.setPropertyPixels(SP_BorderRadius, 10);
    setStyleSheet(ss.toString());

    Q_UNUSED(closeTime)
    // QTimer::singleShot(closeTime.totalMilliseconds(), this, &ToastWidget::onCloseTimer);
#if 0
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
#endif
}

void ToastWidget::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    _label->resize(event->size());
    QPoint pos(event->size().width() - _closeButton->width(), 0);
    _closeButton->move(pos);
    _closeButton->raise();
}

void ToastWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    Q_UNUSED(event)

#if 0
    QPainter painter(this);
    painter.setBrush(_color);
    painter.setOpacity(_opacity);
    painter.fillRect(rect(), _color);
#endif
}

void ToastWidget::mousePressEvent(QMouseEvent* event)
{
    Log::logText(LVL_DEBUG, "Mouse press");
    QFrame::mousePressEvent(event);
}

void ToastWidget::onCloseTimer()
{
    _opacity -= .02;
    if(_opacity <= 0) {
        emit complete();
    }
    else {
        _opacityEffect->setOpacity(_opacity);

        // setWindowOpacity(_opacity);
        update();
        QTimer::singleShot(20, this, &ToastWidget::onCloseTimer);
    }
}

void ToastWidget::onClick()
{
    Log::logText(LVL_DEBUG, "Clicked");
}
