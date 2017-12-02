#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtWebSockets/QWebSocket>

#include "databasecontroller.h"

class Client : public QObject
{
	Q_OBJECT

public:
	explicit Client(DatabaseController *database, QWebSocket *websocket, QObject *parent = nullptr);

	QUuid deviceId() const;

public slots:
	void notifyChanged(const QString &type,
					   const QString &key,
					   bool changed);

signals:
	void connected(const QUuid &userId, bool addClient);

private slots:
	void binaryMessageReceived(const QByteArray &message);
	void error();
	void sslErrors(const QList<QSslError> &errors);
	void closeClient();

private:
	DatabaseController *database;
	QWebSocket *socket;
	QUuid userId;
	QUuid devId;

	QHostAddress socketAddress;
	QAtomicInt runCount;

	void close();
	void sendCommand(const QByteArray &command, const QJsonValue &data = QJsonValue::Null);
	Q_INVOKABLE void doSend(const QByteArray &message);
};

#endif // CLIENT_H
