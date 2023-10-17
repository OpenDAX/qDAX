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

 * Main source code file OpenDAX library interface
 */

#include "dax.h"

/* Dax Class Definitions */
Dax::Dax(const char *name) {
    ds = dax_init(name);
    if(ds == NULL) {
        dax_log(LOG_ERROR, "Unable to Initialize DaxState Object");
    }
}

Dax::~Dax() {
    dax_free(ds);
}


int
Dax::configure(int argc, char **argv, int flags) {
    return dax_configure(ds, argc, argv, flags);
}


int
Dax::connect(void) {
    int result =  dax_connect(ds);
    if(result == ERR_OK) {
       /* No setup work to do here.  We'll go straight to running */
        dax_set_running(ds, 1);
        /* We don't mess with the run/stop/kill callbacks */
        dax_set_default_callbacks(ds);
        dax_set_status(ds, "OK");
        _connected = true;
    } else {
        dax_log(LOG_ERROR, "dax_connect returned %d", result);
    }

    return result;
}


int
Dax::disconnect(void) {
    return dax_disconnect(ds);
}


bool
Dax::isConnected(void) {
    return _connected;
}


int
Dax::getTag(dax_tag *tag, char *name) {
    return dax_tag_byname(ds, tag, name);
}


int
Dax::getTag(dax_tag *tag, tag_index index) {
    return dax_tag_byindex(ds, tag, index);
}


int
Dax::getHandle(tag_handle *h, char *str, int count) {
    return dax_tag_handle(ds, h, str, count);
}


int
Dax::read(tag_handle h, void *data) {
    return dax_tag_read(ds, h, data);
}


int
Dax::write(tag_handle h, void *data, void *mask) {
    if(mask == NULL) {
        return dax_tag_write(ds, h, data);
    } else {
        return dax_tag_mask(ds, h, data, mask);
    }
}


/* Returns a string that represents the given data type.
   If count > 1 then it will add the array index brackets
   to the end of the string. */
std::string *
Dax::typeString(tag_type type, int count) {
    std::string *s;

    s = new std::string(dax_type_to_string(ds, type));
    if(count > 1) {
        *s += "[";
        *s += std::to_string(count);
        *s += "]";
    }
    return s;
}


/* Determine of the given type is a custom (compound) data type */
bool
Dax::isCustom(tag_type type) {
    if(IS_CUSTOM(type)) return true;
    else                return false;
}


static void
_cdt_iter_callback(cdt_iter member, void *udata) {
    std::vector<cdt_iter> *members = (std::vector<cdt_iter> *)udata;
    members->push_back(member);
}


std::vector<cdt_iter>
Dax::getTypeMembers(tag_type type) {
    std::vector<cdt_iter> members;

    dax_cdt_iter(ds, type, &members, _cdt_iter_callback);
    return members;
}


/* The following two functions are static member functions that serve as
   proxies for the user defined callbacks.  The information needed to use
   thse callbacks is stored in the EventUdata structure that is passed as
   the udata here. The ds is ignored. */
void
Dax::_event_callback(dax_state *ds, void *udata) {
    EventUdata *ud = (EventUdata *)udata;
    ud->callback(ud->dax, ud->udata);
}


void
Dax::_free_callback(void *udata) {
    EventUdata *ud = (EventUdata *)udata;
    if(ud->free_callback != NULL) {
        ud->free_callback(ud->udata);
    }
    delete (EventUdata *)udata; /* Simply delete our EventData structure */
}


/* To add an event we store both C callbacks, the userdata and our own pointer into
   an EventUdata structure.  Then we set our own static member callbacks as the actual
   function pointers to libdax.  When we receive the udata pointer back from libdax in
   our callbacks we just call the functions that were stored with the stored data */
int
Dax::eventAdd(tag_handle *handle, int event_type, void *data, dax_id *id, void (*callback)(Dax *dax, void *udata), void *udata, void (*free_callback)(void *udata)) {
    EventUdata *ud = new EventUdata;
    ud->callback = callback;
    ud->free_callback = free_callback;
    ud->udata = udata;
    ud->dax = this;
    return dax_event_add(ds, handle, event_type, data, id, _event_callback, ud, _free_callback);
}


int
Dax::eventOptions(dax_id id, uint32_t options) {
    return dax_event_options(ds, id, options);
}


int
Dax::eventWait(int timeout, dax_id *id) {
    return dax_event_wait(ds, timeout, id);
}


int
Dax::eventPoll(dax_id *id) {
    return dax_event_poll(ds, id);
}


int
Dax::eventGetData(void *buff, int len) {
    return dax_event_get_data(ds, buff, len);
}


std::string
Dax::valueString(tag_type type, void *val, int index) {
    char buff[64];
    int result;

    result = dax_val_to_string(buff, 64, type, val, index);
    return std::string(buff);
}


