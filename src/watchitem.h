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

#ifndef WATCHITEM_H
#define WATCHITEM_H

#include <QObject>
#include <QTreeWidget>
#include "dax.h"

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

#define ITEM_TYPE_ROOT 1001
#define ITEM_TYPE_LEAF 1002

class WatchItem : public QTreeWidgetItem
{
    private:
        static void _update_tag(Dax *d, void *udata);

    protected:
        tag_handle h;
        dax_id event_id;
        void *data;

    public:
        WatchItem(QTreeWidget *parent, QString tagname);
        ~WatchItem();

        tag_handle handle(void) { return h; };
};


#endif