#ifndef GITCREDENTIALRESOLVER_H
#define GITCREDENTIALRESOLVER_H
#include <credentialset.h>
#include <git2qt.h>

class GitCredentialResolver : public GIT::AbstractCredentialResolver
{
public:
    GitCredentialResolver() {}
    GitCredentialResolver(const CredentialSet& credentials) :
        _credentials(credentials) {}

    CredentialSet credentials() const { return _credentials; }
    void setCredentials(CredentialSet value) { _credentials = value; }

public:
    virtual QString getUsername() const override { return _credentials.username(); }
    virtual QString getPassword() const override { return _credentials.password(); }
    virtual QString getPublicKeyFilename() const override { return _credentials.publicKeyFilename(); }
    virtual QString getPrivateKeyFilename() const override { return _credentials.privateKeyFilename(); }

private:
    CredentialSet _credentials;
};

#endif // GITCREDENTIALRESOLVER_H
