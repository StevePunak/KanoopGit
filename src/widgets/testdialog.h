#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>

namespace Ui {
class TestDialog;
}

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestDialog(QWidget *parent = nullptr);
    ~TestDialog();

private:
    Ui::TestDialog *ui;

private slots:
    void onSliderChanged(int value);
};

#endif // TESTDIALOG_H
