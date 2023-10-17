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
    treeWidget->setHeaderLabels(QStringList({"Tagname", "Type", "Value"}));
    tagTimer = new QTimer(this);
    QObject::connect(tagTimer, &QTimer::timeout, this, &MainWindow::updateTags);
    /* Set Tag Tree update buttons */
    toolButtonPlay->setDefaultAction(actionStart_Update);
    toolButtonStop->setDefaultAction(actionStop_Update);
    toolButtonRefresh->setDefaultAction(actionTag_Refresh);
    QObject::connect(actionStart_Update, &QAction::triggered, this, &MainWindow::startTagUpdate);
    QObject::connect(actionStop_Update, &QAction::triggered, this, &MainWindow::stopTagUpdate);
    QObject::connect(actionTag_Refresh, &QAction::triggered, this, &MainWindow::updateTags);
    actionStart_Update->setEnabled(false);
    actionStop_Update->setEnabled(false);
    actionTag_Refresh->setEnabled(false);
    actionConnect->trigger(); /* Try to connect */
    QObject::connect(spinBoxInterval, &QSpinBox::valueChanged, this, &MainWindow::updateTime);
}

MainWindow::~MainWindow() {
    disconnect();
}


void
MainWindow::connect(void) {
    tag_index lastindex;
    tag_handle h;
    int result;

    if( dax.connect() == ERR_OK ) {
        dax_log(LOG_DEBUG, "Connected");
        actionDisconnect->setDisabled(false);
        actionConnect->setDisabled(true);
        result = dax.getHandle(&h, (char *)"_lastindex");
        // TODO deal with error here
        result = dax.read(h, &lastindex);
        // TODO deal with error here
        for(tag_index n = 0; n<=lastindex; n++) {
            addTag(n);
        }
        updateTags();

        eventworker.moveToThread(&eventThread);
        QObject::connect(this, &MainWindow::operate, &eventworker, &EventWorker::go);
        QObject::connect(&eventworker, &EventWorker::tagAdded, this, &MainWindow::addTag);
        QObject::connect(&eventworker, &EventWorker::tagDeleted, this, &MainWindow::delTag);
        eventThread.start();
        emit operate();
        actionStart_Update->setEnabled(true);
        actionTag_Refresh->setEnabled(true);
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
    eventworker.quit();
    eventThread.quit();
    eventThread.wait(2000);
    stopTagUpdate();
    actionStart_Update->setEnabled(false);
    actionStop_Update->setEnabled(false);
    actionTag_Refresh->setEnabled(false);
}


void
MainWindow::addTag(tag_index idx) {
    TagItem *item;
    int result;
    dax_tag tag;

    result = dax.getTag(&tag, idx);
    if(result == ERR_OK) {
        item = new TagItem(static_cast<QTreeWidget *>(nullptr), tag);
        treeWidget->addTopLevelItem(item);
    }
}


void
MainWindow::delTag(tag_index idx) {
    TagItem *item;
    int n;

    for(n=0; n < treeWidget->topLevelItemCount(); n++) {
        item = (TagItem *)treeWidget->topLevelItem(n);
        if(item->handle().index == idx) {
            /* Reading from the deleted tag should clear it from the cache */
            dax.read(item->handle(), item->getData());
            /* This removes it */
            treeWidget->takeTopLevelItem(n);
            return;
        }
    }
}

void
MainWindow::startTagUpdate(void) {
    actionStart_Update->setEnabled(false);
    actionStop_Update->setEnabled(true);
    actionTag_Refresh->setEnabled(false);
    tagTimer->start(spinBoxInterval->value());
}

void
MainWindow::stopTagUpdate(void) {
    actionStart_Update->setEnabled(true);
    actionStop_Update->setEnabled(false);
    actionTag_Refresh->setEnabled(true);
    tagTimer->stop();
}


void
MainWindow::updateTags(void) {
    TagItem *item;
    int result;

    for(int n=0; n < treeWidget->topLevelItemCount(); n++) {
        item = (TagItem *)treeWidget->topLevelItem(n);
        result = dax.read(item->handle(), item->getData());
        item->updateValues();
    }

}

void
MainWindow::updateTime(int msec) {
    tagTimer->setInterval(msec);
}