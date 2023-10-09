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

 * Main source code file OpenDAX tag model item class
 */

#include <QStringList>
#include "dax.h"
#include "tagitem.h"


TagItem::TagItem(const char *name, const char *type, const char *value, TagItem *parentItem)
    : _parentItem(parentItem),
      _name(name),
      _type(type),
      _value(value)
{
    dax_log(LOG_DEBUG, "Constructing item %s", _name);
}


TagItem::~TagItem()
{
    dax_log(LOG_DEBUG, "Destructing item %s", _name);
    clearChildren();
}


void TagItem::appendChild(TagItem *item)
{
    _childItems.append(item);
}


TagItem *TagItem::child(int row)
{
    if (row < 0 || row >= _childItems.size())
        return nullptr;
    return _childItems.at(row);
}


int TagItem::childCount() const
{
    return _childItems.count();
}


int TagItem::row() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<TagItem*>(this));

    return 0;
}


int TagItem::columnCount() const
{
    return 3;
}


QVariant TagItem::data(int column) const
{
    switch(column) {
        case 0:
            return (_name);
        case 1:
            return QVariant(_type);
        case 2:
            return QVariant(_value);
        default:
            return QVariant();
    }
}


TagItem *TagItem::parentItem()
{
    return _parentItem;
}


void TagItem::clearChildren(void) {
    qDeleteAll(_childItems);
    _childItems.clear();
}
