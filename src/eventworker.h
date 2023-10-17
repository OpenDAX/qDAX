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

 *  Header file for event worker thread class
 */

#ifndef EVENTWORKER_H
#define EVENTWORKER_H

#include <QObject>
#include "dax.h"

class EventWorker : public QObject
{
    Q_OBJECT

    private:
        dax_id _tag_added_event_id;
        dax_id _tag_deleted_event_id;
        bool _quit;

        static void _addTagCallback(Dax *dax, void *udata);
        static void _delTagCallback(Dax *dax, void *udata);

    public slots:
        void go(void);
        void quit(void);


    signals:
        void tagAdded(tag_index tag);
        void tagDeleted(tag_index tag);

    public:
        EventWorker();
};

#endif