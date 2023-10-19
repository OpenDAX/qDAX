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

#include <iostream>
#include "mainwindow.h"
#include "dax.h"

extern Dax dax;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    /* GUI Setup */
    dialog = new AboutDialog(this);
    QObject::connect(action_About, &QAction::triggered, dialog, &QDialog::open);
    treeWidget->setColumnCount(3);
    treeWidget->header()->resizeSection(0,200); // Something to save in QSettings
    treeWidget->setHeaderLabels(QStringList({"Tagname", "Type", "Value"}));
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, &QTreeWidget::customContextMenuRequested,
                     this, &MainWindow::treeContextMenu);
    QObject::connect(treeWidget, &QTreeWidget::itemActivated,
                     this, &MainWindow::treeItemActivate);
    lineEditTree->setVisible(false);
    QObject::connect(lineEditTree, &QLineEdit::returnPressed, this, &MainWindow::editAccept);
    toolButtonAccept->setVisible(false);
    toolButtonAccept->setDefaultAction(actionAccept);
    QObject::connect(actionAccept, &QAction::triggered, this, &MainWindow::editAccept);
    /* Tag Update Timer Object */
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
    TagRootItem *item;
    int result;
    dax_tag tag;

    result = dax.getTag(&tag, idx);
    if(result == ERR_OK) {
        item = new TagRootItem(static_cast<QTreeWidget *>(nullptr), tag);
        treeWidget->addTopLevelItem(item);
    }
}


void
MainWindow::delTag(tag_index idx) {
    TagRootItem *item;
    int n;

    for(n=0; n < treeWidget->topLevelItemCount(); n++) {
        item = (TagRootItem *)treeWidget->topLevelItem(n);
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
    TagRootItem *item;
    int result;

    for(int n=0; n < treeWidget->topLevelItemCount(); n++) {
        item = (TagRootItem *)treeWidget->topLevelItem(n);
        result = dax.read(item->handle(), item->getData());
        item->updateValues();
    }

}

void
MainWindow::updateTime(int msec) {
    tagTimer->setInterval(msec);
}

void
MainWindow::aboutDialog(void) {

    dialog->open();
}

void
MainWindow::treeContextMenu(const QPoint& pos) {
    TagBaseItem *item;
    QList<QTreeWidgetItem *> items;
    items = treeWidget->selectedItems();
    QString str = items[0]->data(0, Qt::DisplayRole).toString();

    std::cout << "Context Menu for " << str.toStdString() << std::endl;
}

/* This activates the edit box at the top of the tag view tab*/
void
MainWindow::treeItemActivate(QTreeWidgetItem *item, int column) {
    TagBaseItem *tagitem = (TagBaseItem *)item;
    tag_handle h;
    void *data;

    if(tagitem->writable && !tagitem->readonly) {
        h = tagitem->handle();

        if(h.count > 1 || dax.isCustom(h.type)) return; // Need to deal with CHAR[] at some point

        data = malloc(h.size);
        int result = dax.read(h, data);
        if(result) { free(data); return; } // Probably should indicate this error
        lineEditTree->setText(QString(dax.valueString(h.type, data, 0).c_str()));
        free(data);
        lineEditTree->selectAll();
        lineEditTree->setVisible(true);
        lineEditTree->setFocus(Qt::OtherFocusReason);
        toolButtonAccept->setVisible(true);
    } else {
        lineEditTree->setVisible(false);
        toolButtonAccept->setVisible(false);
    }
}

void
MainWindow::editAccept(void) {
    TagBaseItem *item;
    tag_handle h;
    void *data;
    int result;
    QString str;

    item = (TagBaseItem *)treeWidget->currentItem();
    lineEditTree->setVisible(false);
    toolButtonAccept->setVisible(false);
    treeWidget->setFocus(Qt::OtherFocusReason);

    h = item->handle();
    data = malloc(h.size);
    dax.value(lineEditTree->text().toStdString(), h.type, data, 0);
    result = dax.write(h, data, NULL); /* Write the data to the server */
    if(result) { free(data); return; } // Probably should indicate this error
    result = dax.read(h, data); /* Read it back to make sure */
    if(result) { free(data); return; } // Probably should indicate this error
    if(h.type == DAX_BOOL) {
        if( ((char *)data)[0] ) str = "true";
        else                    str = "false";
    } else {
        str = dax.valueString(h.type, data, 0).c_str();
    }
    item->setData(VALUE_COLUMN, Qt::DisplayRole, str);
    free(data);
}