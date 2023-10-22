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

 *  Source code file for the event worker thread class
 */

#include "qdax.h"
#include "eventworker.h"

extern Dax dax;

EventWorker::EventWorker() {
    _quit = false;
}

void
EventWorker::_addTagCallback(Dax *d, void *udata) {
    tag_index idx;
    int result;

    result = dax.eventGetData(&idx, sizeof(tag_index));
    if(result >= 0) {
        emit ((EventWorker *)udata)->tagAdded(idx);
    }
}


void
EventWorker::_delTagCallback(Dax *d, void *udata) {
    tag_index idx;
    int result;

    result = dax.eventGetData(&idx, sizeof(tag_index));
    if(result >= 0) {
        emit ((EventWorker *)udata)->tagDeleted(idx);
    }
}


void
EventWorker::go(void) {
    tag_handle h;
    dax_id add_id, del_id, id;
    int result;

    result = dax.getHandle(&h, (char *)"_tag_added");
    result = dax.eventAdd(&h, EVENT_WRITE, NULL, &add_id, _addTagCallback, this, NULL);
    result = dax.eventOptions(add_id, EVENT_OPT_SEND_DATA);
    result = dax.getHandle(&h, (char *)"_tag_deleted");
    result = dax.eventAdd(&h, EVENT_WRITE, NULL, &del_id, _delTagCallback, this, NULL);
    result = dax.eventOptions(del_id, EVENT_OPT_SEND_DATA);

    while(!_quit) {
        result = dax.eventWait(500, &id);
    }
    dax.eventDelete(add_id);
    dax.eventDelete(del_id);
    _quit = false;
}

void
EventWorker::quit(void) {
    _quit = true;
}


