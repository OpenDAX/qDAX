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

#include <iostream>
#include "dax.h"

/* Dax Class Definitions */
Dax::Dax(const char *name) {
    ds = dax_init(name);
    if(ds == NULL) {
        std::cerr << "Unable to Initialize DaxState Object\n";
    }
}

Dax::~Dax() {
    dax_free(ds);
}

int Dax::configure(int argc, char **argv, int flags) {
    return dax_configure(ds, argc, argv, flags);
}

int Dax::connect(void) {
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

int Dax::disconnect(void) {
    return dax_disconnect(ds);
}

bool Dax::isConnected(void) {
    return _connected;
}

int Dax::getTag(dax_tag *tag, char *name) {
    return dax_tag_byname(ds, tag, name);
}

int Dax::getTag(dax_tag *tag, tag_index index) {
    return dax_tag_byindex(ds, tag, index);
}

int Dax::getHandle(tag_handle *h, char *str, int count) {
    return dax_tag_handle(ds, h, str, count);
}

int Dax::read(tag_handle h, void *data) {
    return dax_tag_read(ds, h, data);
}

int Dax::write(tag_handle h, void *data, void *mask) {
    if(mask == NULL) {
        return dax_tag_write(ds, h, data);
    } else {
        return dax_tag_mask(ds, h, data, mask);
    }
}

std::string *Dax::typeString(tag_type type, int count) {
    std::string *s;

    s = new std::string(dax_type_to_string(ds, type));
    if(count > 1) {
        *s += "[";
        *s += std::to_string(count);
        *s += "]";
    }
    return s;
}