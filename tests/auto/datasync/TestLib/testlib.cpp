#include "testlib.h"
using namespace QtDataSync;

const QByteArray TestLib::TypeName("TestData");
QTemporaryDir TestLib::tDir;

void TestLib::init()
{
	qRegisterMetaType<TestData>();
	QJsonSerializer::registerListConverters<TestData>();
#ifdef VERBOSE_TESTS
	QLoggingCategory::setFilterRules(QStringLiteral("qtdatasync.*.debug=true"));
#endif
}

Setup &TestLib::setup(Setup &setup)
{
	tDir.setAutoRemove(false);
	return setup.setLocalDir(tDir.path())
			.setKeyStoreProvider(QStringLiteral("plain"))
			.setSignatureScheme(Setup::RSA_PSS_SHA3_512)
			.setSignatureKeyParam(2048)
			.setEncryptionScheme(Setup::RSA_OAEP_SHA3_512)
			.setEncryptionKeyParam(2048);
}

ObjectKey TestLib::generateKey(int index)
{
	return {TypeName, QString::number(index)};
}

TestData TestLib::generateData(int index)
{
	return {index, QString::number(index)};
}

QList<TestData> TestLib::generateData(int from, int to)
{
	QList<TestData> list;
	for(auto i = from; i <= to; i++)
		list.append({i, QString::number(i)});
	return list;
}

QStringList TestLib::generateDataKeys(int from, int to)
{
	QStringList list;
	for(auto i = from; i <= to; i++)
		list.append(QString::number(i));
	return list;
}

QJsonObject TestLib::generateDataJson(int index, const QString &specialText)
{
	QJsonObject data;
	data[QStringLiteral("id")] = index;
	data[QStringLiteral("text")] = specialText.isNull() ? QString::number(index) : specialText;
	return data;
}

TestLib::DataSet TestLib::generateDataJson(int from, int to)
{
	DataSet hash;
	for(auto i = from; i <= to; i++)
		hash.insert(generateKey(i), generateDataJson(i));
	return hash;
}

QJsonArray TestLib::dataListJson(const TestLib::DataSet &data)
{
	QJsonArray v;
	foreach(auto d, data)
		v.append(d);
	return v;
}
