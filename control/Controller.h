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

public:
	bool IsStartService() const;

	void StartService( const std::function<void( bool )> & callback );

	void StopService( const std::function<void( bool )> & callback );

public:
	void GetConfig( const std::function<void( QString )> & callback );

	void SetConfig( const QString & config, const std::function<void( bool )> & callback );

public:
	const QString & GetServer() const;

	void SetServer( const QString & endpoint );

	const QStringList & GetDomainList() const;

	void SetDomainList( const QStringList & domains );

	const QStringList & GetProtocolList() const;

	void SetProtocolList( const QStringList & protocols );

private:
	void ReadConfig();

	void WriteConfig();

private:
	Private * _p;
};