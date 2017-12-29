#ifndef REMOTECONNECTOR_P_H
#define REMOTECONNECTOR_P_H

#include <chrono>

//debugging:
#include <QtCore/QtGlobal>
#ifdef Q_OS_WIN
#pragma message("It's win!")
#ifndef _MSC_VER
#pragma message("_MSC_VER not defined")
#else
#if _MSC_VER < 1900
#pragma message("_MSC_VER < 1900")
#elif _MSC_VER < 1900
#pragma message("_MSC_VER > 1900")
#else
#pragma message("_MSC_VER == 1900")
#endif
#endif
#endif

#include <QtCore/QObject>
#include <QtCore/QUuid>
//fake QT_HAS_INCLUDE macro for QTimer in msvc2015
#if defined(_MSC_VER) && (_MSC_VER == 1900)
#define needs_redef
#undef QT_HAS_INCLUDE
#define QT_HAS_INCLUDE(x) 1
#endif
#include <QtCore/QTimer>
#ifdef needs_redef
#undef QT_HAS_INCLUDE
#define QT_HAS_INCLUDE(x) 0
#endif

#include <QtWebSockets/QWebSocket>

#include "qtdatasync_global.h"
#include "controller_p.h"
#include "defaults.h"
#include "cryptocontroller_p.h"

#include "errormessage_p.h"
#include "identifymessage_p.h"
#include "accountmessage_p.h"
#include "welcomemessage_p.h"
#include "changemessage_p.h"
#include "changedmessage_p.h"

class ConnectorStateMachine;

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

	void initialize(const QVariantHash &params) final;
	void finalize() final;

	Q_INVOKABLE bool isSyncEnabled() const;

public Q_SLOTS:
	void reconnect();
	void disconnect();
	void resync();

	void setSyncEnabled(bool syncEnabled);

	void uploadData(const QByteArray &key, const QByteArray &changeData);
	void downloadDone(const quint64 key);

Q_SIGNALS:
	void finalized();
	void remoteEvent(RemoteEvent event);
	void uploadDone(const QByteArray &key);
	void downloadData(const quint64 key, const QByteArray &changeData);

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
	void onChangeAck(const ChangeAckMessage &message);
	void onChanged(const ChangedMessage &message);
	void onChangedInfo(const ChangedInfoMessage &message);
	void onLastChanged(const LastChangedMessage &message);
};

}

#endif // REMOTECONNECTOR_P_H
