#include "splashscreen.h"

#include "gitassets.h"

#include <Kanoop/gui/resources.h>

#include <QApplication>
#include <QPainter>

SplashScreen::SplashScreen() :
    QSplashScreen{}
{
    QPixmap image = Resources::getPixmap(GitAssets::KanoopGitLogo);

    QPainter painter(&image);
    QPen pen(Qt::white);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::white));

    QFont versionFont("Helvetica", 20);
    versionFont.setBold(true);
    painter.setFont(versionFont);
    painter.drawText(image.rect(), Qt::AlignBottom | Qt::AlignRight, QApplication::instance()->applicationVersion());

    setPixmap(image);
}
