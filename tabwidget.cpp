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

#include "tabwidget.hpp"

#include "connectionwindow.hpp"

#include <QPushButton>

TabWidget::TabWidget()
{
	btnNewTab = new QPushButton(tr("&New"));

	setCornerWidget(btnNewTab);
	setTabsClosable(true);

	connect(
		btnNewTab, &QPushButton::clicked,
		this, &TabWidget::newTab);

	connect(
		this, &TabWidget::tabCloseRequested,
		this, &TabWidget::onTabCloseRequested);

	newTab();
}

void TabWidget::onTabCloseRequested(int index)
{
	ConnectionWindow* cw = qobject_cast<ConnectionWindow*>(widget(index));
	if (!cw) {
		return;
	}

	cw->requestClose();
}

void TabWidget::tabRemoved(int)
{
	if (!count()) {
		newTab();
	}
}

void TabWidget::newTab()
{
	ConnectionWindow* cw = new ConnectionWindow();
	addTab(cw, tr("Unconnected"));
	setCurrentWidget(cw);

	connect(
		cw, &ConnectionWindow::changeTitle,
		this, &TabWidget::onChangeTabTitle);
}

void TabWidget::onChangeTabTitle(const QString& title)
{
	ConnectionWindow* cw = qobject_cast<ConnectionWindow*>(sender());
	if (!cw) {
		return;
	}

	int i = indexOf(cw);
	if (i != -1) {
		setTabText(i, title);
	}
}
