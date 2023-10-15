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

extern Dax dax;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    treeWidget->setColumnCount(3);
    treeWidget->header()->resizeSection(0,200); // Something to save in QSettings
    actionConnect->trigger();
    treeWidget->setHeaderLabels(QStringList({"Tagname", "Type", "Value"}));
}

MainWindow::~MainWindow() {
    disconnect();
}


void
MainWindow::connect(void) {
    int result;
    dax_tag tag;
    tag_index lastindex;
    tag_handle h;

    if( dax.connect() == ERR_OK ) {
        dax_log(LOG_DEBUG, "Connected");
        actionDisconnect->setDisabled(false);
        actionConnect->setDisabled(true);
        result = dax.getHandle(&h, (char *)"_lastindex");
        // TODO deal with error here
        result = dax.read(h, &lastindex);
        // TODO deal with error here
        for(tag_index n = 0; n<=lastindex; n++) {
            result = dax.getTag(&tag, n);
            if(result == ERR_OK) {
                addTag(tag);
            }
        }

        statusbar->showMessage("Connected");
    } else {
        statusbar->showMessage("Failed to Connect");
    }
}


void
MainWindow::disconnect(void) {
    actionConnect->setDisabled(false);
    actionDisconnect->setDisabled(true);
    dax.disconnect();
    dax_log(LOG_DEBUG, "Disconnected");
    statusbar->showMessage("Disconnected");
    treeWidget->clear();
}



void
MainWindow::addTag(dax_tag tag)
{
    TagItem *item;

    item = new TagItem(static_cast<QTreeWidget *>(nullptr), tag);
    treeWidget->addTopLevelItem(item);
}
