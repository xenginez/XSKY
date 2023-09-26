#include "ProtocolDialog.h"

#include "Controller.h"

ProtocolDialog::ProtocolDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	QStringList protocols = Controller::Instance()->GetProtocolList();

	if( protocols.contains( ui.checkBoxDNS->text() ) ) ui.checkBoxDNS->setChecked( true );
	if( protocols.contains( ui.checkBoxmDNS->text() ) ) ui.checkBoxmDNS->setChecked( true );
	if( protocols.contains( ui.checkBoxHTTP->text() ) ) ui.checkBoxHTTP->setChecked( true );
	if( protocols.contains( ui.checkBoxHTTPS->text() ) ) ui.checkBoxHTTPS->setChecked( true );
	if( protocols.contains( ui.checkBoxTCP->text() ) ) ui.checkBoxTCP->setChecked( true );
	if( protocols.contains( ui.checkBoxUDP->text() ) ) ui.checkBoxUDP->setChecked( true );
	if( protocols.contains( ui.checkBoxICMP->text() ) ) ui.checkBoxICMP->setChecked( true );
	if( protocols.contains( ui.checkBoxARP->text() ) ) ui.checkBoxARP->setChecked( true );
}

ProtocolDialog::~ProtocolDialog()
{
}

void ProtocolDialog::on_pushButtonAccept_clicked()
{
	QStringList protocols;

	if( ui.checkBoxDNS->isChecked() ) protocols.push_back( ui.checkBoxDNS->text() );
	if( ui.checkBoxmDNS->isChecked() ) protocols.push_back( ui.checkBoxmDNS->text() );
	if( ui.checkBoxHTTP->isChecked() ) protocols.push_back( ui.checkBoxHTTP->text() );
	if( ui.checkBoxHTTPS->isChecked() ) protocols.push_back( ui.checkBoxHTTPS->text() );
	if( ui.checkBoxTCP->isChecked() ) protocols.push_back( ui.checkBoxTCP->text() );
	if( ui.checkBoxUDP->isChecked() ) protocols.push_back( ui.checkBoxUDP->text() );
	if( ui.checkBoxICMP->isChecked() ) protocols.push_back( ui.checkBoxICMP->text() );
	if( ui.checkBoxARP->isChecked() ) protocols.push_back( ui.checkBoxARP->text() );

	Controller::Instance()->SetProtocolList( protocols );

	accept();
}

void ProtocolDialog::on_pushButtonReject_clicked()
{
	reject();
}
