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

 *  Header file for the tag add dialog box
 */

#ifndef _ADD_TYPE_DIALOG_H
#define _ADD_TYPE_DIALOG_H

#include "ui_addtypedialog.h"
#include "dax.h"

class TypeItem : public QTreeWidgetItem
{
    protected:

    public:
        QString name, typestr;
        tag_type type;
        uint32_t count;

        TypeItem(QTreeWidget *parent, QString name, tag_type type, uint32_t count);

};


class AddTypeDialog : public QDialog, public Ui_AddTypeDialog
{
    Q_OBJECT

    private:
        void addMember(void);

    public:
        explicit AddTypeDialog(QWidget *parent = nullptr);
        ~AddTypeDialog();

    public slots:


    signals:

};

#endif