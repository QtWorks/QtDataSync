#ifndef PLAINKEYSTORE_H
#define PLAINKEYSTORE_H

#include <QtCore/QSettings>
#include <QtCore/QPointer>

#include <QtDataSync/keystore.h>

class PlainKeyStore : public QtDataSync::KeyStore
{
	Q_OBJECT

public:
	explicit PlainKeyStore(QObject *parent = nullptr);

	bool loadStore() override;
	void closeStore() override;
	bool containsSecret(const QString &key) const override;
	bool storeSecret(const QString &key, const QByteArray &secret) override;
	QByteArray loadSecret(const QString &key) override;
	bool removeSecret(const QString &key) override;

private:
	QPointer<QSettings> _settings;
};

#endif // PLAINKEYSTORE_H