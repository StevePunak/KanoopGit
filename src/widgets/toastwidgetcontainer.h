#ifndef TOASTWIDGETCONTAINER_H
#define TOASTWIDGETCONTAINER_H

#include <QDateTime>
#include <QFrame>

class Label;
class QToolButton;
class QLabel;
class ToastWidgetContainer : public QFrame
{
    Q_OBJECT
public:
    explicit ToastWidgetContainer(QWidget *parent = nullptr);
    void displayToast(const QString& text, const QColor& color);

private:
    void performLayout();

signals:

};

class ToastWidget : public QFrame
{
    Q_OBJECT
public:
    ToastWidget(const QString& text, const QColor& color, QWidget* parent = nullptr);

    QDateTime createTime() const { return _createTime; }

private:
    Label* _label;
    QToolButton* _closeButton;
    QDateTime _createTime;
};

#endif // TOASTWIDGETCONTAINER_H
