#ifndef TOASTWIDGETCONTAINER_H
#define TOASTWIDGETCONTAINER_H

#include <QDateTime>
#include <QFrame>

#include <Kanoop/timespan.h>

class QGraphicsOpacityEffect;
class Label;
class QToolButton;
class QLabel;
class ToastWidgetContainer : public QFrame
{
    Q_OBJECT
public:
    explicit ToastWidgetContainer(QWidget *parent = nullptr);
    void displayToast(const QString& text, const QColor& color);

    TimeSpan autoCloseTime() const { return _autoCloseTime; }
    void setAutoCloseTime(const TimeSpan& value) { _autoCloseTime = value; }

private:
    void performLayout();

    TimeSpan _autoCloseTime;

signals:

private slots:
    void closeToast();
};

class ToastWidget : public QFrame
{
    Q_OBJECT
public:
    ToastWidget(const QString& text, const QColor& color, const TimeSpan& closeTime, QWidget* parent = nullptr);

    QDateTime createTime() const { return _createTime; }

private:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

    Label* _label;
    QToolButton* _closeButton;
    QColor _color;
    QDateTime _createTime;
    QGraphicsOpacityEffect* _opacityEffect = nullptr;
    double _opacity = 1.0;

signals:
    void complete();

private slots:
    void onCloseTimer();
    void onClick();
};

#endif // TOASTWIDGETCONTAINER_H
