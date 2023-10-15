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

 *  Source code file for tag item
 */

#include <iostream>
#include "tagitem.h"

extern Dax dax;


TagItem::TagItem(QTreeWidget *parent, dax_tag tag) : QTreeWidgetItem(parent)
{
    QString typestr;
    QString tagname;

    setData(NAME_COLUMN, Qt::DisplayRole, tag.name);
    setData(TYPE_COLUMN, Qt::DisplayRole, dax.typeString(tag.type, tag.count)->c_str());

    dax.getHandle(&h, tag.name);
    data = malloc(h.size);

    if(tag.count > 1) {
        typestr = dax.typeString(tag.type)->c_str();
        for(int i=0;i<tag.count;i++) {
            addArrayItem(this, tag.name, tag.type, i);
        }
    } else {
        if(dax.isCustom(tag.type)) {
            addCDTItems(this, tag.name, tag.type);
        }
    }
}

TagItem::~TagItem()
{
    if(data != NULL) {
        free(data);
    }
}

void
TagItem::addArrayItem(QTreeWidgetItem *item, char * name, tag_type type, int index) {
    QString typestr;
    QString tagname;
    QTreeWidgetItem *child;


    typestr = dax.typeString(type)->c_str();
    tagname = QString(name) + "[" + QString::number(index) + "]";
    child = new QTreeWidgetItem(item, QStringList({tagname, typestr, ""}));
    item->addChild(child);
    if(dax.isCustom(type)) {
        addCDTItems(child, (char *)tagname.toStdString().c_str(), type);
    }
}

void
TagItem::addCDTItems(QTreeWidgetItem *item, char * name, tag_type type) {
    QString typestr;
    QString tagname;
    QTreeWidgetItem *child;

    std::vector<cdt_iter> members = dax.getTypeMembers(type);
    for(auto m : members) {
        typestr = dax.typeString(m.type, m.count)->c_str();
        tagname = QString(name) + "." + QString(m.name);
        child = new QTreeWidgetItem(item, QStringList({tagname, typestr, ""}));

        item->addChild(child);

        if(m.count > 1) {
            typestr = dax.typeString(m.type)->c_str();
            for(int i=0;i<m.count;i++) {
                addArrayItem(child, (char *)tagname.toStdString().c_str(), m.type, i);
            }
        }
    }
}