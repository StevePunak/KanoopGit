#ifndef CREDENTIALSWIDGET_H
#define CREDENTIALSWIDGET_H

#include <QWidget>
#include <credentialset.h>

namespace Ui {
class CredentialsWidget;
}

class CredentialsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CredentialsWidget(QWidget *parent = nullptr);
    ~CredentialsWidget();

    QString credentialsName() const { return _credentialsName; }
    void setCredentialsName(const QString& value) { _credentialsName = value; }

    CredentialSet credentials() const;
    void setCredentials(const CredentialSet& credentials);

private:
    Ui::CredentialsWidget *ui;

    QString _credentialsName;

private slots:
    void onBrowsePublicKeyClicked();
    void onBrowsePrivateKeyClicked();
};

#endif // CREDENTIALSWIDGET_H
