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

 *  Source code file for the about dialog box
 */

#include <iostream>
#include "addtypedialog.h"
#include "dax.h"
#include <config.h>

extern Dax dax;


TypeItem::TypeItem(QTreeWidget *parent, QString name, tag_type type, uint32_t count) : QTreeWidgetItem(parent) {
    this->name = name;
    this->type = type;
    this->count = count;

    typestr = dax.typeString(type, count)->c_str();
    setData(0, Qt::DisplayRole, name);
    setData(1, Qt::DisplayRole, typestr);
}


AddTypeDialog::AddTypeDialog(QWidget *parent) : QDialog(parent) {
    std::vector<type_id> types;

    setupUi(this);

    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels(QStringList({"Name", "Type"}));
    treeWidget->setItemsExpandable(false);
    treeWidget->setRootIsDecorated(false);

    QObject::connect(pushButtonAdd, &QPushButton::clicked, this, &AddTypeDialog::addMember);

    types = dax.getTypes();
    for(type_id type : types) {
        comboBoxType->addItem(QString(type.name.c_str()), type.type);
    }

    lineEditName->setFocus(Qt::OtherFocusReason);

}

AddTypeDialog::~AddTypeDialog() {
    ;
}

void
AddTypeDialog::addMember(void) {
    QTreeWidgetItem *item;
    QString name;
    QString typestr;
    tag_type type;
    int count;

    name = lineEditMemberName->text();
    type = comboBoxType->currentData().toInt();
    count = spinBoxCount->value();
    lineEditMemberName->setFocus(Qt::OtherFocusReason);
    lineEditMemberName->selectAll();

    item = new TypeItem(treeWidget, name, type, count);
    treeWidget->addTopLevelItem(item);
}