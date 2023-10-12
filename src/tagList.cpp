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

 *  Source code file for tag list
 */

#include "tagList.h"



TagList::TagList(Dax& d) : dax(d)
{
    tree = NULL;
}

void TagList::setTreeWidget(QTreeWidget *tw)
{
    tree = tw;

}

void TagList::connect(void) {
    tag_handle h;
    dax_tag tag;
    int result;
    tag_index lastindex;
    QString typestr;
    QString tagname;

    result = dax.getHandle(&h, (char *)"_lastindex");
    // TODO deal with error here
    result = dax.read(h, &lastindex);
    // TODO deal with error here
    for(tag_index n = 0; n<=lastindex; n++) {
        result = dax.getTag(&tag, n);
        if(result == ERR_OK) {
            addTag(tag);
            // item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList({tag.name, dax.typeString(tag.type, tag.count)->c_str(), ""}));
            // items.append(item);
            // if(tag.count > 1) {
            //     typestr = dax.typeString(tag.type)->c_str();
            //     for(int i=0;i<tag.count;i++) {
            //         tagname = QString(tag.name) + "[" + QString::number(i) + "]";
            //         item->addChild(new QTreeWidgetItem(item, QStringList({tagname, typestr, ""})));
            //     }
            // }
        }
    }
    //tree->insertTopLevelItems(0, items);
}

void TagList::disconnect(void) {
    if(tree)
        tree->clear();
}

void TagList::addTag(dax_tag tag)
{
    TagItem *item;

    item = new TagItem();
    item->treeitem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList({tag.name, dax.typeString(tag.type, tag.count)->c_str(), ""}));
    item->name = new QString(tag.name);
    tree->addTopLevelItem(item->treeitem);
    dax.getHandle(&item->h, tag.name);
    item->data = malloc(item->h.size);
    taglist.append(item);
}

