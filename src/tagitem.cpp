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

#include "tagitem.h"

extern Dax dax;

TagLeafItem::TagLeafItem(QTreeWidgetItem *parent, QString tagname, QString typestr) : QTreeWidgetItem(parent) {
    setData(NAME_COLUMN, Qt::DisplayRole, tagname);
    setData(TYPE_COLUMN, Qt::DisplayRole, typestr);

    int result = dax.getHandle(&h, (char *)tagname.toStdString().c_str());
    if(result) {
        dax_log(LOG_ERROR, "Unable to get tag handle ");
    }
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


TagItem::TagItem(QTreeWidget *parent, dax_tag tag) : QTreeWidgetItem(parent)
{
    QString typestr;
    QString tagname;

    setData(NAME_COLUMN, Qt::DisplayRole, tag.name);
    setData(TYPE_COLUMN, Qt::DisplayRole, dax.typeString(tag.type, tag.count)->c_str());

    int result = dax.getHandle(&h, tag.name);
    if(result) {
        dax_log(LOG_ERROR, "Unable to get tag handle ");
    }
    _data = malloc(h.size);

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
    if(_data != NULL) {
        free(_data);
    }
}


void
TagItem::addArrayItem(QTreeWidgetItem *item, char * name, tag_type type, int index) {
    QString typestr;
    QString tagname;
    QTreeWidgetItem *child;

    typestr = dax.typeString(type)->c_str();
    tagname = QString(name) + "[" + QString::number(index) + "]";
    child = new TagLeafItem(item, tagname, typestr);
    item->addChild(child);
    if(dax.isCustom(type)) {
        addCDTItems(child, tagname, type);
    }
}


void
TagItem::addCDTItems(QTreeWidgetItem *item, QString name, tag_type type) {
    QString typestr;
    QString tagname;
    QTreeWidgetItem *child;

    std::vector<cdt_iter> members = dax.getTypeMembers(type);
    for(auto m : members) {
        typestr = dax.typeString(m.type, m.count)->c_str();
        tagname = QString(name) + "." + QString(m.name);
        child = new TagLeafItem(item, tagname, QString(typestr));

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
TagItem::updateValues(void) {
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
        //valstr = dax.valueString(h.type, _data, 0);
        //setData(VALUE_COLUMN, Qt::DisplayRole, QString(valstr.c_str()));
    }
}