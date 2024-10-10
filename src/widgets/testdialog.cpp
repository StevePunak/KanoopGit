#include "testdialog.h"
#include "ui_testdialog.h"

TestDialog::TestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestDialog)
{
    ui->setupUi(this);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &TestDialog::onSliderChanged);
    connect(ui->checkBox, &QCheckBox::toggled, ui->spinner, &SpinnerWidget::setSpinning);
    ui->spinner->setFixedSize(25, 25);
}

TestDialog::~TestDialog()
{
    delete ui;
}

void TestDialog::onSliderChanged(int value)
{
    ui->spinner->setValue(value);
}
