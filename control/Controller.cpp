#include "Controller.h"

#include <QQueue>
#include <QTimer>
#include <QUdpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#define ROUTE_HOST QHostAddress("3.3.3.3")
#define ROUTE_PORT 3333

struct Controller::Private
{
	bool IsStart = false;

	QString Server = "127.0.0.1:48888";
	QStringList Domains;
	QStringList Protocols;

	QUdpSocket ReadSocket, WriteSocket;
	QQueue< std::function< void() > > Callbacks;
};

Controller::Controller()
	:_p( new Private )
{
	_p->ReadSocket.bind( ROUTE_PORT );

	connect( &_p->ReadSocket, &QUdpSocket::readyRead, [this]()
	{
		auto sz = _p->ReadSocket.pendingDatagramSize();

		QByteArray buf( sz, 0 );
		_p->ReadSocket.readDatagram( buf.data(), buf.size() );

		QJsonDocument doc = QJsonDocument::fromJson( buf );
		{

		}

		_p->Callbacks.front()( );
		_p->Callbacks.pop_front();
	} );
}

Controller::~Controller()
{
	delete _p;
}

Controller * Controller::Instance()
{
	static Controller c;
	return &c;
}

bool Controller::IsStartService() const
{
	return _p->IsStart;
}

void Controller::StartService( const std::function<void( bool )> & callback )
{
	if( _p->IsStart == false )
	{
		_p->IsStart = true;

		WriteConfig();
	}

	callback( true );
}

void Controller::StopService( const std::function<void( bool )> & callback )
{
	if( _p->IsStart == true )
	{
		_p->IsStart = false;

		WriteConfig();
	}

	callback( true );
}

void Controller::GetConfig( const std::function<void( QString )> & callback )
{
	_p->WriteSocket.writeDatagram( "{}", ROUTE_HOST, ROUTE_PORT );
}

void Controller::SetConfig( const QString & config, const std::function<void( bool )> & callback )
{
	_p->WriteSocket.writeDatagram( config.toUtf8(), ROUTE_HOST, ROUTE_PORT );
}

const QString & Controller::GetServer() const
{
	return _p->Server;
}

void Controller::SetServer( const QString & endpoint )
{
	if( _p->Server != endpoint )
	{
		_p->Server = endpoint;

		WriteConfig();
	}
}

const QStringList & Controller::GetDomainList() const
{
	return _p->Domains;
}

void Controller::SetDomainList( const QStringList & domains )
{
	if( _p->Domains != domains )
	{
		_p->Domains = domains;

		WriteConfig();
	}
}

const QStringList & Controller::GetProtocolList() const
{
	return _p->Protocols;
}

void Controller::SetProtocolList( const QStringList & protocols )
{
	if( _p->Protocols != protocols )
	{
		_p->Protocols = protocols;

		WriteConfig();
	}
}

void Controller::ReadConfig()
{
}

void Controller::WriteConfig()
{
	QJsonObject obj;
	obj.insert( "Start", _p->IsStart );
	obj.insert( "Server", _p->Server );
	QJsonArray domains;
	for( auto it : _p->Domains )
	{
		domains.push_back( it );
	}
	obj.insert( "Domains", domains );
	QJsonArray protocols;
	for( auto it : _p->Protocols )
	{
		domains.push_back( it );
	}
	obj.insert( "Protocols", protocols );

	_p->WriteSocket.writeDatagram( QJsonDocument( obj ).toJson(), ROUTE_HOST, ROUTE_PORT );
}
