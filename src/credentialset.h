#ifndef CREDENTIALSET_H
#define CREDENTIALSET_H
#include <Kanoop/serialization/serializablejsonlist.h>

class CredentialSet : public ISerializableToJsonObject,
                      public IDeserializableFromJsonObject
{
public:
    CredentialSet() {}

    virtual QJsonObject serializeToJsonObject() const override;
    virtual void deserializeFromJsonObject(const QJsonObject& jsonObject) override;

    QString name() const { return _name; }
    void setName(const QString& value) { _name = value; }

    QString username() const { return _username; }
    void setUsername(const QString& value) { _username = value; }

    QString password() const { return _password; }
    void setPassword(const QString& value) { _password = value; }

    QString publicKeyFilename() const { return _publicKeyFilename; }
    void setPublicKeyFilename(const QString& value) { _publicKeyFilename = value; }

    QString privateKeyFilename() const { return _privateKeyFilename; }
    void setPrivateKeyFilename(const QString& value) { _privateKeyFilename = value; }

    bool isValid() const { return _name.isEmpty() == false; }

    void fromDataStream(QDataStream& in);
    void toDataStream(QDataStream& out) const;

    class List : public SerializableJsonList<CredentialSet>,
                 public ISerializableToJson,
                 public IDeserializableFromJson
    {
    public:
        virtual QByteArray serializeToJson() const override;
        virtual void deserializeFromJson(const QByteArray& json) override;

        CredentialSet findByName(const QString& name) const
        {
            CredentialSet result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const CredentialSet& a) { return a.name() == name; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        void saveCredentials(const CredentialSet& credentialSet)
        {
            auto it = std::find_if(begin(), end(), [credentialSet](const CredentialSet& a) { return a.name() == credentialSet.name(); } );
            if(it != end()) {
                *it = credentialSet;
            }
            else {
                append(credentialSet);
            }
        }
    };

    static const QString DefaultName;

private:
    QString _name;
    QString _username;
    QString _password;
    QString _publicKeyFilename;
    QString _privateKeyFilename;
};

QDataStream &operator<<(QDataStream &out, const CredentialSet &config);
QDataStream &operator>>(QDataStream &in, CredentialSet &config);

Q_DECLARE_METATYPE(CredentialSet)

#endif // CREDENTIALSET_H
