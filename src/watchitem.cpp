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

 *  Source code file for tag watch item
 */

#include <iostream>
#include "qdax.h"
#include "watchitem.h"

extern Dax dax;

WatchItem::WatchItem(QTreeWidget *parent, QString tagname) : QTreeWidgetItem(parent) {
    std::string valstr;
    int result;

    setData(0, Qt::DisplayRole, tagname);
    result = dax.getHandle(&h, (char *)tagname.toStdString().c_str());
    if(result) {
        throw result;
    }
    if(h.count > 1 && h.type != DAX_CHAR) {
        throw ERR_2BIG;
    }
    DF("count = %d", h.count);
    data = malloc(h.size);
    dax.read(h, data);

    result = dax.eventAdd(&h, EVENT_CHANGE, NULL, &event_id, _update_tag, this, NULL);
    if(result) {
        free(data);
        throw result;
    }
    result = dax.eventOptions(event_id, EVENT_OPT_SEND_DATA);
    if(result) {
        free(data);
        throw result;
    }

    if(h.count > 1 && h.type==DAX_CHAR) {
        for(int n=0; n<h.count;n++) {
            valstr += ((char *)data)[n];
        }
        setData(1, Qt::DisplayRole, QString(valstr.c_str()));
    } else {
        if(h.type == DAX_BOOL) {
            if(((uint8_t *)data)[h.byte] & (uint8_t)(0x01 << h.bit))
                valstr = "true";
            else
                valstr = "false";
        } else {
            valstr = dax.valueString(h.type, &((char *)data)[h.byte], 0);
        }
        setData(1, Qt::DisplayRole, QString(valstr.c_str()));
    }
}


void
WatchItem::_update_tag(Dax *d, void *udata) {
    std::string valstr;
    WatchItem *item = (WatchItem *)udata;

    dax.eventGetData(item->data, item->h.size);
    if(item->h.count > 1 && item->h.type==DAX_CHAR) {
        for(int n=0; n<item->h.count;n++) {
            valstr += ((char *)item->data)[n];
        }
        item->setData(1, Qt::DisplayRole, QString(valstr.c_str()));
    } else {
        if(item->h.type == DAX_BOOL) {
            if(((uint8_t *)item->data)[item->h.byte] & (uint8_t)(0x01 << item->h.bit))
                valstr = "true";
            else
                valstr = "false";
        } else {
            valstr = dax.valueString(item->h.type, &((char *)item->data)[item->h.byte], 0);
        }
        item->setData(1, Qt::DisplayRole, QString(valstr.c_str()));
    }
}


WatchItem::~WatchItem() {
    dax.eventDelete(event_id);
    free(data);
}
