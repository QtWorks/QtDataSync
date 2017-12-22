#ifndef REMOTECONNECTOR_P_H
#define REMOTECONNECTOR_P_H

#include <chrono>

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QTimer>

#include <QtWebSockets/QWebSocket>

#include "qtdatasync_global.h"
#include "controller_p.h"
#include "defaults.h"
#include "cryptocontroller_p.h"

#include "errormessage_p.h"
#include "identifymessage_p.h"
#include "accountmessage_p.h"
#include "welcomemessage_p.h"
#include "donemessage_p.h"

#include "connectorstatemachine.h"

namespace QtDataSync {

class Q_DATASYNC_EXPORT RemoteConnector : public Controller
{
	Q_OBJECT

	Q_PROPERTY(bool syncEnabled READ isSyncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)

public:
	static const QString keyRemoteEnabled;
	static const QString keyRemoteUrl;
	static const QString keyAccessKey;
	static const QString keyHeaders;
	static const QString keyKeepaliveTimeout;
	static const QString keyDeviceId;
	static const QString keyDeviceName;

	enum RemoteEvent {
		RemoteDisconnected,
		RemoteConnecting,
		RemoteReady,
		RemoteReadyWithChanges
	};
	Q_ENUM(RemoteEvent)

	explicit RemoteConnector(const Defaults &defaults, QObject *parent = nullptr);

	void initialize() final;
	void finalize() final;

	Q_INVOKABLE bool isSyncEnabled() const;

public Q_SLOTS:
	void reconnect();
	void disconnect();
	void resync();

	void setSyncEnabled(bool syncEnabled);

	void uploadData(const QByteArray &key, const QByteArray &changeData);

Q_SIGNALS:
	void finalized();
	void remoteEvent(RemoteEvent event);
	void uploadDone(const QByteArray &key);

	void syncEnabledChanged(bool syncEnabled);

private Q_SLOTS:
	void connected();
	void disconnected();
	void binaryMessageReceived(const QByteArray &message);
	void error(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);
	void ping();

	//statemachine
	void doConnect();
	void doDisconnect();
	void scheduleRetry();

private:
	static const QVector<std::chrono::seconds> Timeouts;

	CryptoController *_cryptoController;

	QWebSocket *_socket;

	QTimer *_pingTimer;
	bool _awaitingPing;

	ConnectorStateMachine *_stateMachine;
	int _retryIndex;
	bool _expectChanges;

	QUuid _deviceId;

	bool isIdle() const;
	void triggerError(bool canRecover);

	bool checkCanSync(QUrl &remoteUrl);
	bool loadIdentity();
	void tryClose();
	std::chrono::seconds retry();

	QVariant sValue(const QString &key) const;

	void onError(const ErrorMessage &message);
	void onIdentify(const IdentifyMessage &message);
	void onAccount(const AccountMessage &message);
	void onWelcome(const WelcomeMessage &message);
	void onDone(const DoneMessage &message);
};

}

#endif // REMOTECONNECTOR_P_H
