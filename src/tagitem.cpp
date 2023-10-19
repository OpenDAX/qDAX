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

TagBaseItem::TagBaseItem(QTreeWidget *parent, int type) : QTreeWidgetItem(parent, type) {
}

TagBaseItem::TagBaseItem(QTreeWidgetItem *parent, int type) : QTreeWidgetItem(parent, type) {
}




TagLeafItem::TagLeafItem(QTreeWidgetItem *parent, QString tagname, QString typestr) : TagBaseItem(parent, ITEM_TYPE_LEAF) {
    setData(NAME_COLUMN, Qt::DisplayRole, tagname);
    setData(TYPE_COLUMN, Qt::DisplayRole, typestr);

    int result = dax.getHandle(&h, (char *)tagname.toStdString().c_str());
    if(result) {
        dax_log(LOG_ERROR, "Unable to get tag handle ");
    }
    //std::cout << tagname.toStdString() << " byte = " << h.byte << std::endl;
}


void
TagLeafItem::updateValues(void *data) {
    std::string valstr;
    TagLeafItem *item;

    if(h.count > 1) {
        if(h.type == DAX_CHAR) {
            for(int n=0;n<h.count;n++) {
                valstr += (&((char *)data)[h.byte])[n];
            }
            setData(VALUE_COLUMN, Qt::DisplayRole, QString(valstr.c_str()));
        }
        for(int n=0;n<childCount();n++) {
            item = (TagLeafItem *)child(n);
            item->updateValues(data);
        }
    } else if(dax.isCustom(h.type)) {
        for(int n=0;n<childCount();n++) {
            item = (TagLeafItem *)child(n);
            item->updateValues(data);
        }
    } else {
        if(h.type == DAX_BOOL) {
            if(((uint8_t *)data)[h.byte] & (uint8_t)(0x01 << h.bit))
                valstr = "true";
            else
                valstr = "false";
        } else {
            valstr = dax.valueString(h.type, &((char *)data)[h.byte], 0);
        }
        setData(VALUE_COLUMN, Qt::DisplayRole, QString(valstr.c_str()));

    }
}


TagRootItem::TagRootItem(QTreeWidget *parent, dax_tag tag) : TagBaseItem(parent, ITEM_TYPE_ROOT)
{
    QString typestr;
    QString tagname;

    setData(NAME_COLUMN, Qt::DisplayRole, tag.name);
    setData(TYPE_COLUMN, Qt::DisplayRole, dax.typeString(tag.type, tag.count)->c_str());

    int result = dax.getHandle(&h, tag.name);
    if(result) {
        dax_log(LOG_ERROR, "Unable to get tag handle ");
    }
    if(tag.attr & TAG_ATTR_READONLY) readonly = true;
    _data = malloc(h.size);

    if(tag.count > 1) {
        if(tag.type != DAX_CHAR) writable = false;
        typestr = dax.typeString(tag.type)->c_str();
        for(int i=0;i<tag.count;i++) {
            addArrayItem(this, tag.name, tag.type, i);
        }
    } else {
        if(dax.isCustom(tag.type)) {
            writable = false;
            addCDTItems(this, tag.name, tag.type);
        }
    }
}


TagRootItem::~TagRootItem()
{
    if(_data != NULL) {
        free(_data);
    }
}


void
TagRootItem::addArrayItem(QTreeWidgetItem *item, char * name, tag_type type, int index) {
    QString typestr;
    QString tagname;
    TagLeafItem *child;

    typestr = dax.typeString(type)->c_str();
    tagname = QString(name) + "[" + QString::number(index) + "]";
    child = new TagLeafItem(item, tagname, typestr);
    if(readonly) child->readonly = true;
    item->addChild(child);
    if(dax.isCustom(type)) {
        addCDTItems(child, tagname, type);
    }
}


void
TagRootItem::addCDTItems(QTreeWidgetItem *item, QString name, tag_type type) {
    QString typestr;
    QString tagname;
    TagLeafItem *child;

    std::vector<cdt_iter> members = dax.getTypeMembers(type);
    for(auto m : members) {
        typestr = dax.typeString(m.type, m.count)->c_str();
        tagname = QString(name) + "." + QString(m.name);
        child = new TagLeafItem(item, tagname, QString(typestr));
        if(readonly) child->readonly = true;
        item->addChild(child);

        if(m.count > 1) {
            typestr = dax.typeString(m.type)->c_str();
            for(int i=0;i<m.count;i++) {
                addArrayItem(child, (char *)tagname.toStdString().c_str(), m.type, i);
            }
        }
    }
}


void
TagRootItem::updateValues(void) {
    std::string valstr;
    TagLeafItem *item;

    if(h.count > 1) {
        if(h.type == DAX_CHAR) {
            for(int n=0;n<h.count;n++) {
                valstr += ((char *)_data)[n];
            }
            setData(VALUE_COLUMN, Qt::DisplayRole, QString(valstr.c_str()));
        }
        for(int n=0;n<childCount();n++) {
            item = (TagLeafItem *)child(n);
            item->updateValues(_data);
        }
    } else if(dax.isCustom(h.type)) {
        for(int n=0;n<childCount();n++) {
            item = (TagLeafItem *)child(n);
            item->updateValues(_data);
        }
    } else {
        if(h.type == DAX_BOOL) {
            if(((uint8_t *)_data)[h.byte] & (uint8_t)(0x01 << h.bit))
                valstr = "true";
            else
                valstr = "false";
        } else {
            valstr = dax.valueString(h.type, &((char *)_data)[h.byte], 0);
        }
        setData(VALUE_COLUMN, Qt::DisplayRole, QString(valstr.c_str()));
    }
}