#ifndef GITAPPLICATION_H
#define GITAPPLICATION_H

#include <Kanoop/application.h>

class GitApplication : public Application
{
    Q_OBJECT
public:
    GitApplication(int& argc, char *argv[]);

public slots:
    void onPreferencesChanged();
};

#endif // GITAPPLICATION_H
