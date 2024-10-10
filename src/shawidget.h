#ifndef SHAWIDGET_H
#define SHAWIDGET_H

#include <QLabel>
#include <QToolButton>
#include <QWidget>

#include <git2qt.h>

class ShaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShaWidget(const GIT::ObjectId& objectId, QWidget *parent = nullptr);

private:
    GIT::ObjectId _objectId;

    QLabel* _shaLabel;
    QToolButton* _copyButton;

signals:

private slots:
    void onCopyClicked();
};

#endif // SHAWIDGET_H
