#ifndef PARENTSSHAWIDGET_H
#define PARENTSSHAWIDGET_H

#include <QLabel>
#include <QWidget>
#include <git2qt.h>

class ParentsShaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParentsShaWidget(const GIT::ObjectId::List& objectIds, QWidget *parent = nullptr);

signals:
    void clicked(const GIT::ObjectId& objectId);
};

namespace TW {

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(const GIT::ObjectId& objectId, QWidget* parent = nullptr);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;

private:
    GIT::ObjectId _objectId;

signals:
    void clicked(const GIT::ObjectId& objectId);
};

} // namespace

#endif // PARENTSSHAWIDGET_H
