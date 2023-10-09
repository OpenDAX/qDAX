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

 * Header file OpenDAX tag model item class
 */

#ifndef TAGITEM_H
#define TAGITEM_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "dax.h"

class TagItem
{
public:
    explicit TagItem(const char *name, const char *type, const char *value, TagItem *parentItem = nullptr);
    ~TagItem();

    void appendChild(TagItem *child);

    TagItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TagItem *parentItem();
    void clearChildren(void);

private:
    QList<TagItem *> _childItems;
    QString _name;
    QString _type;
    QString _value;
    TagItem *_parentItem;
};

#endif