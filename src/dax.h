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

 * Header file OpenDAX library interface
 */

#ifndef DAX_H
#define DAX_H

#include <opendax.h>
#include <vector>
#include <string>

struct type_id {
    std::string name;
    tag_type type;
    uint32_t count;
};


class Dax
{
    private:
        bool _connected;
        dax_state *ds;

        static void _event_callback(dax_state *ds, void *udata);
        static void _free_callback(void *udata);

    public:
        Dax(const char *name);
        ~Dax();
        int configure(int argc, char **argv, int flags);
        int connect(void);
        int disconnect(void);
        bool isConnected(void);
        int tagAdd(tag_handle *h, std::string name, tag_type type, uint32_t count, uint32_t attr=0x00);
        int tagDel(tag_index index);
        int tagDel(std::string name);
        int getTag(dax_tag *tag, char *name);
        int getTag(dax_tag *tag, tag_index index);
        int getHandle(tag_handle *h, char *str, int count = 0);
        int read(tag_handle h, void *data);
        int write(tag_handle h, void *data, void *mask = NULL);
        std::string *typeString(tag_type type, int count = 1);
        bool isCustom(tag_type type);
        int typeAdd(std::string name, std::vector<type_id> members, tag_type *type = NULL);
        std::vector<cdt_iter> getTypeMembers(tag_type type);
        std::vector<type_id> getTypes(void);
        int eventAdd(tag_handle *handle, int event_type, void *data, dax_id *id,
                     void (*callback)(Dax *dax, void *udata), void *udata,
                     void (*free_callback)(void *udata));
        int eventDelete(dax_id);
        int eventOptions(dax_id id, uint32_t options);
        int eventWait(int timeout, dax_id *id);
        int eventPoll(dax_id *id);
        int eventGetData(void *buff, int len);
        std::string valueString(tag_type type, void *val, int index);
        int value(std::string str, tag_type type, void *val, int index);

};

struct EventUdata {
    void (*callback)(Dax *dax, void *udata);
    void (*free_callback)(void *udata);
    void *udata;
    Dax *dax;
};


#endif