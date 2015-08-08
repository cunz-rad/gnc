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


#pragma once

#include <QWidget>
#include <QDateTime>
#include <QTcpSocket>
#include <QStringBuilder>

class QLineEdit;
class QSpinBox;
class QTextBrowser;
class QPushButton;
class QLabel;

enum class DataType
{
	SentFromServer,
	SentFromClient,
	Status,
	Error
};

struct DataExchange
{
	bool			isCached		: 1;
	int				id;
	DataType		type;
	QDateTime		timeStamp;
	QString			rawLine;
	QString			htmlCache;

	QString anchor() const {
		return QLatin1String("id") % QString::number(id);
	}
};

using DataExchangeRows = QVector<DataExchange>;

class ConnectionWindow
	: public QWidget
{
	Q_OBJECT
private:
	enum class State {
		None,

		Disconnected,
		Connecting,
		Connected,
		Disconnecting,
	};

public:
	ConnectionWindow();

public:
	void requestClose();

private:
	void setupUi();
	void rebuildHtml(const QString& anchor = QString());

signals:
	void changeTitle(const QString& title);

private slots:
	void onConnected();
	void onDisconnected();
	void onReadyRead();
	void onBtnSend();
	void onBtnConnect();
	void onSocketError(QAbstractSocket::SocketError);

private:
	void setState(State s);
	void doConnect();

private:
	State				state;
	QLabel*				lblStatus;
	QLineEdit*			txtSvrName;
	QSpinBox*			txtPort;
	QTextBrowser*		txtDisplay;
	QLineEdit*			txtInput;
	QPushButton*		btnSend;
	QPushButton*		btnConnect;
	QTcpSocket*			socket;
	DataExchangeRows	rows;
	QByteArray			inBuffer;
	QString				htmlCache;
};