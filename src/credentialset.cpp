#include "credentialset.h"

#include <QJsonDocument>

const QString CredentialSet::DefaultName        = "default";

QJsonObject CredentialSet::serializeToJsonObject() const
{
    QJsonObject jsonObject;
    jsonObject["name"] = _name;
    jsonObject["username"] = _username;
    jsonObject["password"] = _password;
    jsonObject["ssh_pub"] = _publicKeyFilename;
    jsonObject["ssh_priv"] = _privateKeyFilename;
    return jsonObject;
}

void CredentialSet::deserializeFromJsonObject(const QJsonObject& jsonObject)
{
    _name = jsonObject["name"].toString();
    _username = jsonObject["username"].toString();
    _password = jsonObject["password"].toString();
    _publicKeyFilename = jsonObject["ssh_pub"].toString();
    _privateKeyFilename = jsonObject["ssh_priv"].toString();
}

void CredentialSet::fromDataStream(QDataStream& in)
{
    in >> _name
        >> _username
        >> _password
        >> _publicKeyFilename
        >> _privateKeyFilename;
}

void CredentialSet::toDataStream(QDataStream& out) const
{
    out << _name
        << _username
        << _password
        << _publicKeyFilename
        << _privateKeyFilename;
}

QByteArray CredentialSet::List::serializeToJson() const
{
    QJsonDocument doc;
    QJsonArray a = serializeToJsonArray();
    doc.setArray(a);
    return doc.toJson(QJsonDocument::Compact);
}

void CredentialSet::List::deserializeFromJson(const QByteArray& json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json);
    deserializeFromJsonArray(doc.array());
}

QDataStream& operator<<(QDataStream& out, const CredentialSet& config) { config.toDataStream(out); return out; }

QDataStream& operator>>(QDataStream& in, CredentialSet& config) { config.fromDataStream(in); return in; }
