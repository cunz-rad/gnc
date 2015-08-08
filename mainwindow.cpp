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

#include <QVBoxLayout>
#include <QSplitter>

#include "mainwindow.hpp"
#include "connectionwindow.hpp"
#include "tabwidget.hpp"

MainWindow::MainWindow()
	: QWidget()
{
	setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
	QSplitter* s = new QSplitter;

	TabWidget* tabs = new TabWidget();
	s->addWidget(tabs);
	
	QVBoxLayout* l = new QVBoxLayout();
	l->setSpacing(0);

	l->addWidget(s);
	setLayout(l);

	resize(1024, 768);

	setWindowTitle(tr("Graphical Net Cat"));
}
