/*  qDAX - An open source data acquisition and control system
 *  Copyright (c) 2023 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *

 *  Source code file for main window class
 */

#include "mainwindow.h"
#include "dax.h"

MainWindow::MainWindow(Dax& d, QWidget *parent) : QMainWindow(parent), dax(d) {
    setupUi(this);
    _taglist = new TagList(d);
    treeWidget->setColumnCount(3);
    _taglist->setTreeWidget(treeWidget);
    actionConnect->trigger();
    treeWidget->setHeaderLabels(QStringList({"Tagname", "Type", "Value"}));
}

MainWindow::~MainWindow() {
    disconnect();
}


void
MainWindow::connect(void) {
    if( dax.connect() == ERR_OK ) {
        dax_log(LOG_DEBUG, "Connected");
        actionDisconnect->setDisabled(false);
        actionConnect->setDisabled(true);
        _taglist->connect();
    }
}


void
MainWindow::disconnect(void) {
    actionConnect->setDisabled(false);
    actionDisconnect->setDisabled(true);
    dax.disconnect();
    _taglist->disconnect();
    dax_log(LOG_DEBUG, "Disconnected");
}

