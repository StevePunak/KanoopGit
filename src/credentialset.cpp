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
