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
#include <QMessageBox>

extern Dax dax;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    /* GUI Setup */
    _aboutDialog = new AboutDialog(this);
    QObject::connect(action_About, &QAction::triggered, _aboutDialog, &QDialog::open);
    treeWidget->setColumnCount(3);
    treeWidget->header()->resizeSection(0,200); // Something to save in QSettings
    treeWidget->setHeaderLabels(QStringList({"Tagname", "Type", "Value"}));
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, &QTreeWidget::customContextMenuRequested,
                     this, &MainWindow::treeContextMenu);
    QObject::connect(treeWidget, &QTreeWidget::itemActivated,
                     this, &MainWindow::treeItemActivate);
    QObject::connect(treeWidget, &QTreeWidget::currentItemChanged,
                    this, &MainWindow::treeItemChanged);
    lineEditTree->setVisible(false);
    QObject::connect(lineEditTree, &QLineEdit::returnPressed, this, &MainWindow::editAccept);
    toolButtonAccept->setVisible(false);
    toolButtonAccept->setDefaultAction(actionAccept);
    QObject::connect(actionAccept, &QAction::triggered, this, &MainWindow::editAccept);
    QObject::connect(actionAdd_Tag, &QAction::triggered, this, &MainWindow::addTag);
    QObject::connect(actionDelete_Tag, &QAction::triggered, this, &MainWindow::deleteTag);
    QObject::connect(actionAdd_Type, &QAction::triggered, this, &MainWindow::addType);
    QObject::connect(actionAdd_To_Watchlist, &QAction::triggered, this, &MainWindow::addToWatchlist);
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
    actionAdd_To_Watchlist->setEnabled(false);
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
            addTagToTree(n);
        }
        updateTags();
        eventThread = new QThread();
        eventworker = new EventWorker();
        eventworker->moveToThread(eventThread);
        QObject::connect(this, &MainWindow::operate, eventworker, &EventWorker::go);
        QObject::connect(eventworker, &EventWorker::tagAdded, this, &MainWindow::addTagToTree);
        QObject::connect(eventworker, &EventWorker::tagDeleted, this, &MainWindow::delTagFromTree);
        eventThread->start();
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
    eventworker->quit();
    eventThread->quit();
    eventThread->wait(2000);
    delete eventThread;
    delete eventworker;
    dax.disconnect();
    dax_log(LOG_DEBUG, "Disconnected");
    statusbar->showMessage("Disconnected");
    treeWidget->clear();
    stopTagUpdate();
    actionStart_Update->setEnabled(false);
    actionStop_Update->setEnabled(false);
    actionTag_Refresh->setEnabled(false);
}


void
MainWindow::addTagToTree(tag_index idx) {
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
MainWindow::delTagFromTree(tag_index idx) {
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

    _aboutDialog->open();
}

void
MainWindow::treeContextMenu(const QPoint& pos) {
    TagBaseItem *item;
    QList<QTreeWidgetItem *> items;
    QMenu menu;

    items = treeWidget->selectedItems();
    if(items.size() > 0) {
        item = (TagBaseItem *)items[0];
        QString str = items[0]->data(0, Qt::DisplayRole).toString();

        std::cout << "Context Menu for " << str.toStdString() << std::endl;
        menu.addAction(actionDelete_Tag);
        menu.addAction(actionAdd_To_Watchlist);
        menu.addSeparator();
        menu.addAction(actionTag_Info);
        menu.exec(treeWidget->mapToGlobal(pos));
    }
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

/* This gets called any time we changed the selected item in the tree.  It's
   mainly for updating actions depending on what is selected */
void
MainWindow::treeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    TagBaseItem *item = (TagBaseItem *)current;
    if(item) {
        if(item->writable) actionAdd_To_Watchlist->setEnabled(true);
        else actionAdd_To_Watchlist->setEnabled(false);
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

void
MainWindow::addTag(void) {
    AddTagDialog d(this);
    std::vector<type_id> types;
    std::string tagname, str;
    tag_type tagType;
    uint32_t count;
    int result;

    types = dax.getTypes();
    for(type_id type : types) {
        d.comboBoxType->addItem(QString(type.name.c_str()), type.type);
    }
    d.lineEditName->setFocus(Qt::OtherFocusReason);
    result = d.exec();
    if(result == QDialog::Accepted) {
        tagname = d.lineEditName->text().toStdString();
        tagType = (tag_type)d.comboBoxType->currentData().toInt();
        count = d.spinBoxCount->value();
        result = dax.tagAdd(NULL, tagname, tagType, count);
        if(result == ERR_OK) {
            str = std::string("Tag '") + tagname + "' Added";
            statusbar->showMessage(str.c_str());
        } else {
            // TODO: This should be a message box
            statusbar->showMessage("Failed to Add Tag");
        }
    }
}

void
MainWindow::deleteTag(void) {
    TagBaseItem *item;
    QMessageBox msgBox(this);
    tag_index idx;
    int result;

    if(tabWidget->currentIndex() == 0) {
        item = (TagBaseItem *)treeWidget->currentItem();
        /* This loop takes us back to the root tag item */
        while(item->type() == ITEM_TYPE_LEAF) item = (TagBaseItem *)item->parent();
        idx = item->handle().index;
        QString tagname = item->data(0, Qt::DisplayRole).toString();
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setText(QString("Are you sure you want to delete tag '" + tagname + "'?"));
        msgBox.setWindowTitle(QString("Delete Tag"));
        result = msgBox.exec();
        if(result == QMessageBox::Yes) {
            result = dax.tagDel(idx);
            if(result == ERR_OK) {
                statusbar->showMessage("Tag Deleted Successfully");
            } else {
                statusbar->showMessage("Error deleting tag");
            }
        }
    } else if(tabWidget->currentIndex() == 1) {
        std::cout << "Find on Watch tab" << std::endl;
    } else {
        std::cout << "Oops" << std::endl;
    }
}

void
MainWindow::addType(void) {
    int result;
    AddTypeDialog d(this);
    TypeItem *item;
    std::vector<type_id> members;
    type_id t;

    result = d.exec();
    // TODO: Need to do a bunch of checking here for bad names.
    if(QDialog::Accepted) {
        for(int i=0; i < d.treeWidget->topLevelItemCount(); i++) {
            item = (TypeItem *)d.treeWidget->topLevelItem(i);
            std::cout << item->name.toStdString();
            t.name = item->name.toStdString();
            t.type = item->type;
            t.count = item->count;
            members.push_back(t);
        }
        result = dax.typeAdd(d.lineEditName->text().toStdString(), members);
        if(result == ERR_OK) {
            statusbar->showMessage("Type Created");
        } else {
            // TODO: Better error message here
            statusbar->showMessage("Failed to create type");
        }
    }

}

void
MainWindow::addToWatchlist(void) {
    std::cout << "Watch Tag" << std::endl;
}
