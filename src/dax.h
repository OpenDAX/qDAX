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


class Dax
{
    public:
        Dax(const char *name);
        ~Dax();
        int configure(int argc, char **argv, int flags);
        int connect(void);
        int disconnect(void);
        bool isConnected(void);
        int getTag(dax_tag *tag, char *name);
        int getTag(dax_tag *tag, tag_index index);
        int getHandle(tag_handle *h, char *str, int count = 1);
        int read(tag_handle h, void *data);
        int write(tag_handle h, void *data, void *mask = NULL);
        std::string *typeString(tag_type type, int count = 1);
    private:
        bool _connected;
        dax_state *ds;
};

#endif