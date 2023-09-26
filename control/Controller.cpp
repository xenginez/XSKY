#include "Controller.h"

#include <QQueue>
#include <QTimer>
#include <QSettings>
#include <QUdpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "../common/common.h"

struct Controller::Private
{
	bool IsStart = false;

	QString Server = "127.0.0.1:48888";
	QStringList Domains;
	QStringList Protocols;

	QUdpSocket ReadSocket, WriteSocket;
};

Controller::Controller()
	:_p( new Private )
{
	ReadConfig( {} );

	_p->ReadSocket.bind( CONFIG_PORT );
	connect( &_p->ReadSocket, &QUdpSocket::readyRead, [this]()
	{
		auto sz = _p->ReadSocket.pendingDatagramSize();

		QByteArray buf( sz, 0 );
		_p->ReadSocket.readDatagram( buf.data(), buf.size() );

		ReadConfig( buf );
	} );

	_p->WriteSocket.writeDatagram( "{}", QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );
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

void Controller::StartService()
{
	if( _p->IsStart == false )
	{
		_p->IsStart = true;

		_p->WriteSocket.writeDatagram( WriteConfig(), QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );

		emit configChanged();
	}
}

void Controller::StopService()
{
	if( _p->IsStart == true )
	{
		_p->IsStart = false;

		_p->WriteSocket.writeDatagram( WriteConfig(), QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );

		emit configChanged();
	}
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

		_p->WriteSocket.writeDatagram( WriteConfig(), QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );

		emit configChanged();
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

		_p->WriteSocket.writeDatagram( WriteConfig(), QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );

		emit configChanged();
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

		_p->WriteSocket.writeDatagram( WriteConfig(), QHostAddress( ROUTE_ADDRESS ), CONFIG_PORT );

		emit configChanged();
	}
}

void Controller::ReadConfig( const QByteArray & data )
{
	if( !data.isEmpty() )
	{
		auto obj = QJsonDocument::fromJson( data ).object();
		_p->IsStart = obj["Start"].toBool();
		_p->Server = obj["Server"].toString();
		QJsonArray domains = obj["Domains"].toArray();
		for( auto it : domains )
		{
			domains.push_back( it.toString() );
		}
		QJsonArray protocols = obj["Protocols"].toArray();
		for( auto it : protocols )
		{
			domains.push_back( it.toString() );
		}
	}
	else
	{
		int count = 0;
		QSettings settings( "config.ini", QSettings::IniFormat );
		settings.beginGroup( "Controller" );
		{
			_p->IsStart = settings.value( "Start", false ).toBool();
			_p->Server = settings.value( "Server", "127.0.0.1:48888" ).toString();

			count = settings.beginReadArray( "Domains" );
			for( int i = 0; i < count; i++ )
			{
				settings.setArrayIndex( i );
				_p->Domains.push_back( settings.value( "domain", "" ).toString() );
			}
			settings.endArray();

			count = settings.beginReadArray( "Protocols" );
			for( int i = 0; i < count; i++ )
			{
				settings.setArrayIndex( i );
				_p->Protocols.push_back( settings.value( "protocol", "" ).toString() );
			}
			settings.endArray();
		}
		settings.endGroup();
	}

	emit configChanged();
}

QByteArray Controller::WriteConfig()
{
	QSettings settings( "config.ini", QSettings::IniFormat );
	settings.beginGroup( "Controller" );
	{
		settings.setValue( "Start", _p->IsStart );
		settings.setValue( "Server", _p->Server );

		settings.beginWriteArray( "Domains" );
		for( int i = 0; i < _p->Domains.size(); i++ )
		{
			settings.setArrayIndex( i );
			settings.setValue( "domain", _p->Domains[i] );
		}
		settings.endArray();

		settings.beginWriteArray( "Protocols" );
		for( int i = 0; i < _p->Protocols.size(); i++ )
		{
			settings.setArrayIndex( i );
			settings.setValue( "protocol", _p->Protocols[i] );
		}
		settings.endArray();
	}
	settings.endGroup();

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

	return  QJsonDocument( obj ).toJson();
}
