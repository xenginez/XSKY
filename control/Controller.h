#pragma once

#include <string>
#include <functional>

#include <QObject>
#include <QHostInfo>

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

	void captureItem( const QByteArray & item );

public:
	bool IsStartService() const;

	void StartService();

	void StopService();

public:
	bool IsCapture() const;

	void StartCapture( const QString & setting );

	void StopCapture();

public:
	const QString & GetServer() const;

	void SetServer( const QString & endpoint );

	const QStringList & GetDomainList() const;

	void SetDomainList( const QStringList & domains );

	const QStringList & GetDeviceList() const;

	void SetDeviceList( const QStringList & devices );

	const QStringList & GetProtocolList() const;

	void SetProtocolList( const QStringList & protocols );

private:
	void ReadConfig( const QByteArray & data );

	QByteArray WriteConfig();

private slots:
	void OnLookupHost( const QHostInfo & host );

private:
	Private * _p;
};