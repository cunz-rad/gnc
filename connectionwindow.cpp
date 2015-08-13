/*
	Graphical Net Cat (GNC)
	Copyright(C) 2015 Sascha Cunz <sascha@cunz-rad.com>
	Copyright(C) 2015 Cunz RaD Ltd.

	This program is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.If not, see <http://www.gnu.org/licenses/>.
*/


#include "connectionwindow.hpp"

#include <QPushButton>
#include <QTextBrowser>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>

#include <QTcpSocket>

namespace {

	QString socketErrorText(QAbstractSocket::SocketError e)
	{
		switch (e) {
		case QAbstractSocket::ConnectionRefusedError:			return QStringLiteral("Connection refused");
		case QAbstractSocket::RemoteHostClosedError:			return QStringLiteral("Remote host closed");
		case QAbstractSocket::HostNotFoundError:				return QStringLiteral("Host not found");
		case QAbstractSocket::SocketAccessError:				return QStringLiteral("Socket access error");
		case QAbstractSocket::SocketResourceError:				return QStringLiteral("Socket resource");
		case QAbstractSocket::SocketTimeoutError:				return QStringLiteral("Socket timeout");
		case QAbstractSocket::DatagramTooLargeError:			return QStringLiteral("Datagram too large");
		case QAbstractSocket::NetworkError:						return QStringLiteral("Network");
		case QAbstractSocket::AddressInUseError:				return QStringLiteral("Address in use");
		case QAbstractSocket::SocketAddressNotAvailableError:	return QStringLiteral("Socket address not available");
		case QAbstractSocket::UnsupportedSocketOperationError:	return QStringLiteral("Unsupported socket operation");
		case QAbstractSocket::UnfinishedSocketOperationError:	return QStringLiteral("Unfinished socket operation");
		case QAbstractSocket::ProxyAuthenticationRequiredError:	return QStringLiteral("Proxy authentification required");
		case QAbstractSocket::SslHandshakeFailedError:			return QStringLiteral("SSL handshake failed");
		case QAbstractSocket::ProxyConnectionRefusedError:		return QStringLiteral("Proxy connection refused");
		case QAbstractSocket::ProxyConnectionClosedError:		return QStringLiteral("Proxy connection closed");
		case QAbstractSocket::ProxyConnectionTimeoutError:		return QStringLiteral("Proxy connection timeout");
		case QAbstractSocket::ProxyNotFoundError:				return QStringLiteral("Proxy not found");
		case QAbstractSocket::ProxyProtocolError:				return QStringLiteral("Proxy protocol error");
		case QAbstractSocket::OperationError:					return QStringLiteral("Operation error");
		case QAbstractSocket::SslInternalError:					return QStringLiteral("SSL internal error");
		case QAbstractSocket::SslInvalidUserDataError:			return QStringLiteral("SSL invalid user data error");
		case QAbstractSocket::TemporaryError:					return QStringLiteral("Temporary error");
		default:
		case QAbstractSocket::UnknownSocketError:				return QStringLiteral("Unknown socket error");
		}
	}
}

ConnectionWindow::ConnectionWindow()
	: state(State::None)
	, socket(nullptr)
{
	setupUi();
}

void ConnectionWindow::setupUi()
{
	lblStatus = new QLabel;
	lblStatus->setVisible(false);
	txtSvrName = new QLineEdit;
	txtPort = new QSpinBox;
	txtPort->setMaximum(0xFFFF);
	txtPort->setMinimum(1);
	btnConnect = new QPushButton(tr("&Connect"));

	QHBoxLayout* h1 = new QHBoxLayout;
	h1->addWidget(lblStatus, 1);
	h1->addWidget(txtSvrName, 1);
	h1->addWidget(txtPort);
	h1->addWidget(btnConnect);
	h1->setMargin(0);

	txtDisplay = new QTextBrowser;

	txtInput = new QLineEdit;
	btnSend = new QPushButton(tr("&Send"));
	QHBoxLayout* h2 = new QHBoxLayout;
	h2->addWidget(txtInput, 1);
	h2->addWidget(btnSend);
	h2->setMargin(0);

	QVBoxLayout* v = new QVBoxLayout;
	v->addLayout(h1);
	v->addWidget(txtDisplay);
	v->addLayout(h2);
	//v->setMargin(0);
	setLayout(v);

	connect(
		btnConnect, &QPushButton::clicked,
		this, &ConnectionWindow::onBtnConnect);

	connect(
		btnSend, &QPushButton::clicked,
		this, &ConnectionWindow::onBtnSend);

	connect(
		txtInput, &QLineEdit::returnPressed,
		this, &ConnectionWindow::onBtnSend);

	setState(State::Disconnected);
}

void ConnectionWindow::rebuildHtml(const QString& anchor)
{
	QString html = QStringLiteral(
		"<style>\n"
			"td { font-size: 12pt; }\n"
			".svr { font-family: \"Courier New\", monospace; color: blue; }\n"
			".clt { font-family: \"Courier New\", monospace; color: black; }\n"
			".err { color: red; }\n"
			".ctl { color: green; }\n"
		"</style>\n"
		"<table>");

	for(DataExchange& de : rows) {
		if (!de.isCached) {
			QString style;
			switch (de.type) {
			case DataType::SentFromServer:
				style = QStringLiteral("svr");
				break;

			case DataType::SentFromClient:
				style = QStringLiteral("clt");
				break;
			case DataType::Error:
				style = QStringLiteral("err");
				break;

			case DataType::Status:
				style = QStringLiteral("ctl");
				break;
			}

			de.htmlCache = QStringLiteral(
				"<tr><td id=\"%1\">%2</td><td class=\"%3\">%4</td></tr>"
				)
				.arg(de.anchor())
				.arg(de.timeStamp.toString("HH:mm:ss.zzz"))
				.arg(style)
				.arg(de.rawLine.toHtmlEscaped())
				;
			de.isCached = true;
		}
		html += de.htmlCache;
	}

	html += QStringLiteral("</table>");
	htmlCache = html;
	txtDisplay->setHtml(html);

	if (!anchor.isEmpty()) {
		txtDisplay->scrollToAnchor(anchor);
	}
}

void ConnectionWindow::onConnected()
{
	setState(State::Connected);
	DataExchange de;
	de.isCached = false;
	de.type = DataType::Status;
	de.rawLine = tr("Connection established");
	de.timeStamp = QDateTime::currentDateTime();
	de.id = rows.count();
	rows.append(de);
	rebuildHtml();
}

void ConnectionWindow::onDisconnected()
{
	setState(State::Disconnected);
	DataExchange de;
	de.isCached = false;
	de.type = DataType::Status;
	de.rawLine = tr("Connection lost");
	de.timeStamp = QDateTime::currentDateTime();
	de.id = rows.count();
	rows.append(de);
	rebuildHtml(de.anchor());
}

void ConnectionWindow::onReadyRead()
{
	QString lastAnchor;
	bool didChange = false;
	int idx;

	while (socket->bytesAvailable() > 0)
	{
		inBuffer += socket->readAll();
	}

	while ((idx = inBuffer.indexOf('\n')) >=0 ) {
		QByteArray ba = inBuffer.left(idx);
		inBuffer = inBuffer.mid(idx + 1);
		DataExchange de;
		de.isCached = false;
		de.type = DataType::SentFromServer;
		de.rawLine = QString::fromUtf8(ba);
		de.timeStamp = QDateTime::currentDateTime();
		de.id = rows.count();
		rows.append(de);
		lastAnchor = de.anchor();
		didChange = true;
	}

	if (didChange) {
		rebuildHtml(lastAnchor);
	}
}

void ConnectionWindow::onBtnSend()
{
	QString raw = txtInput->text();

	if (raw.isEmpty()) {
		// return;
	}

	DataExchange de;
	de.isCached = false;
	de.type = DataType::SentFromClient;
	de.rawLine = raw;
	de.timeStamp = QDateTime::currentDateTime();
	de.id = rows.count();
	rows.append(de);

	QByteArray send = raw.toUtf8() + '\n';
	socket->write(send);

	rebuildHtml(de.anchor());

	txtInput->setText(QString());
	txtInput->setFocus();
}

void ConnectionWindow::onBtnConnect()
{
	switch (state) {
	case State::Disconnected:
		doConnect();
		break;

	case State::Connected:
		setState(State::Disconnecting);
		socket->disconnectFromHost();
		break;

	default:
		return;
	}
}

void ConnectionWindow::setState(State s)
{
	if (s == state) {
		return;
	}

	switch (s) {
	case State::Connecting:
		lblStatus->setText(tr("Connecting to <b>%1:%2</b>.").arg(txtSvrName->text()).arg(txtPort->value()));
		break;
	case State::Connected:
		lblStatus->setText(tr("Connected to <b>%1:%2</b>.").arg(txtSvrName->text()).arg(txtPort->value()));
		emit changeTitle(tr("%1:%2").arg(txtSvrName->text()).arg(txtPort->value()));
		btnConnect->setText(tr("Disc&onnect"));
		break;
	case State::Disconnecting:
		lblStatus->setText(tr("Disconnecting from <b>%1:%2</b>.").arg(txtSvrName->text()).arg(txtPort->value()));
		break;
	case State::Disconnected:
		emit changeTitle(tr("Disconnected"));
		btnConnect->setText(tr("C&onnect"));
		break;

	case State::None:
		Q_ASSERT(false);
		return;
	}

	lblStatus->setVisible(s != State::Disconnected);
	txtSvrName->setVisible(s == State::Disconnected);
	txtPort->setVisible(s == State::Disconnected);
	btnConnect->setEnabled(s == State::Disconnected || s == State::Connected);
	txtInput->setVisible(s == State::Connected);
	btnSend->setVisible(s == State::Connected);

	state = s;
}

void ConnectionWindow::requestClose()
{
	deleteLater();
}

void ConnectionWindow::onSocketError(QAbstractSocket::SocketError e)
{
	DataExchange de;
	de.isCached = false;
	de.type = DataType::Error;
	de.rawLine = tr("Error: %1").arg(socketErrorText(e));
	de.timeStamp = QDateTime::currentDateTime();
	de.id = rows.count();
	rows.append(de);
	rebuildHtml(de.anchor());
}

void ConnectionWindow::doConnect()
{
	QString svr = txtSvrName->text();
	int port = txtPort->value();

	socket = new QTcpSocket(this);

	connect(
		socket, &QTcpSocket::connected,
		this, &ConnectionWindow::onConnected);
	connect(
		socket, &QTcpSocket::disconnected,
		this, &ConnectionWindow::onDisconnected);
	connect(
		socket, &QTcpSocket::readyRead,
		this, &ConnectionWindow::onReadyRead);

	connect(	// We don't want that reinterpret_castery, so do it old school:
		socket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(onSocketError(QAbstractSocket::SocketError)));

	socket->connectToHost(svr, port);

	setState(State::Connecting);
}