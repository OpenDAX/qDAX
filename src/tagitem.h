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

 *  Header file for tag item classes that represent the items in the tag Tree
 */

#ifndef TAGLIST_H
#define TAGLIST_H

#include <QObject>
#include <QTreeWidget>
#include "dax.h"

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

#define ITEM_TYPE_ROOT 1001
#define ITEM_TYPE_LEAF 1002

class TagBaseItem : public QTreeWidgetItem
{
    protected:
        tag_handle h;

    public:
        bool writable = true;
        bool readonly = false;

        TagBaseItem(QTreeWidget *parent, int type);
        TagBaseItem(QTreeWidgetItem *parent, int type);

        tag_handle handle(void) { return h; };
};


class TagLeafItem : public TagBaseItem
{
    public:
        TagLeafItem(QTreeWidgetItem *parent, QString tagname, QString typestr);
        void updateValues(void *data);

};


class TagRootItem : public TagBaseItem
{
    private:
        void *_data;

    public:
        TagRootItem(QTreeWidget *parent, dax_tag tag);
        ~TagRootItem();
        void addArrayItem(QTreeWidgetItem *item, char * name, tag_type type, int index);
        void addCDTItems(QTreeWidgetItem *item, QString name, tag_type type);
        void *getData(void) { return _data; };
        void updateValues(void);
};


#endif