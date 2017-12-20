#ifndef LOCALSTORE_P_H
#define LOCALSTORE_P_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QCache>
#include <QtCore/QReadWriteLock>
#include <QtCore/QJsonObject>

#include <QtSql/QSqlDatabase>

#include "qtdatasync_global.h"
#include "objectkey.h"
#include "defaults.h"
#include "logger.h"
#include "exception.h"

namespace QtDataSync {

class Q_DATASYNC_EXPORT LocalStoreEmitter : public QObject
{
	Q_OBJECT

public:
	LocalStoreEmitter(QObject *parent = nullptr);

Q_SIGNALS:
	void dataChanged(QObject *origin, const QtDataSync::ObjectKey &key, const QJsonObject data, int size);
	void dataResetted(QObject *origin, const QByteArray &typeName = {});
};

class Q_DATASYNC_EXPORT LocalStore : public QObject //TODO use const where useful
{
	Q_OBJECT

	Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize RESET resetCacheSize)

public:
	explicit LocalStore(QObject *parent = nullptr);
	explicit LocalStore(const QString &setupName, QObject *parent = nullptr);
	~LocalStore();

	static QDir typeDirectory(Defaults defaults, const ObjectKey &key);
	static QJsonObject readJson(Defaults defaults, const ObjectKey &key, const QString &fileName, int *costs = nullptr);

	quint64 count(const QByteArray &typeName);
	QStringList keys(const QByteArray &typeName);
	QList<QJsonObject> loadAll(const QByteArray &typeName);

	QJsonObject load(const ObjectKey &key);
	void save(const ObjectKey &key, const QJsonObject &data);
	bool remove(const ObjectKey &key);

	QList<QJsonObject> find(const QByteArray &typeName, const QString &query);
	void clear(const QByteArray &typeName);
	void reset();

	int cacheSize() const;

public Q_SLOTS:
	void setCacheSize(int cacheSize);
	void resetCacheSize();

Q_SIGNALS:
	void dataChanged(const QtDataSync::ObjectKey &key, bool deleted);
	void dataCleared(const QByteArray &typeName);
	void dataResetted();

private Q_SLOTS:
	void onDataChange(QObject *origin, const QtDataSync::ObjectKey &key, const QJsonObject &data, int size);
	void onDataReset(QObject *origin, const QByteArray &typeName);

private:
	Defaults _defaults;
	Logger *_logger;

	DatabaseRef _database;

	QHash<QByteArray, QString> _tableNameCache;
	QCache<ObjectKey, QJsonObject> _dataCache;

	void exec(QSqlQuery &query, const ObjectKey &key = ObjectKey{"any"}) const;
	QDir typeDirectory(const ObjectKey &key);
	QJsonObject readJson(const ObjectKey &key, const QString &fileName, int *costs = nullptr);
};

}

#endif // LOCALSTORE_P_H
