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
 * Main source code file OpenDAX tag data model
 */

#include <QStringList>
#include "dax.h"
#include "tagmodel.h"

TagModel::TagModel(Dax& d, QObject *parent)
    : QAbstractItemModel(parent), dax(d)
{
    rootItem = new TagItem("Name", "Type", "Value");
}

TagModel::~TagModel()
{
    delete rootItem;
}

QModelIndex TagModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TagItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TagItem*>(parent.internalPointer());

    TagItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TagModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TagItem *childItem = static_cast<TagItem*>(index.internalPointer());
    TagItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TagModel::rowCount(const QModelIndex &parent) const
{
    TagItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TagItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TagModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TagItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TagModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TagItem *item = static_cast<TagItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TagModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TagModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

void TagModel::init(void) {
    tag_handle h;
    dax_tag tag;
    int result;
    tag_index lastindex;
    TagItem *t;

    result = dax.getHandle(&h, (char *)"_lastindex");
    // TODO deal with error here
    result = dax.read(h, &lastindex);
    // TODO deal with error here
    for(tag_index n = 0; n<=lastindex; n++) {
        result = dax.getTag(&tag, n);
        if(result == ERR_OK) {
            t = new TagItem(tag.name, dax.typeString(tag.type, tag.count)->c_str(), "", rootItem);
            rootItem->appendChild(t);
        }
    }

}

void TagModel::clear(void) {
    rootItem->clearChildren();
    emit this->dataChanged(QModelIndex(), QModelIndex());
}
