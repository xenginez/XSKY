#pragma once

#include <string>
#include <functional>

#include <QObject>

class Controller : public QObject
{
	Q_OBJECT

private:
	struct Private;

private:
	Controller();

	~Controller();

public:
	static Controller * Instance();

signals:
	void configChanged();

public:
	bool IsStartService() const;

	void StartService();

	void StopService();

public:
	const QString & GetServer() const;

	void SetServer( const QString & endpoint );

	const QStringList & GetDomainList() const;

	void SetDomainList( const QStringList & domains );

	const QStringList & GetProtocolList() const;

	void SetProtocolList( const QStringList & protocols );

private:
	void ReadConfig( const QByteArray & data );

	QByteArray WriteConfig();

private:
	Private * _p;
};