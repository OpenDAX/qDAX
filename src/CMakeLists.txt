#  Copyright (c) 2023 Phil Birkelbach
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

configure_file(config.h.in config.h)

qt_add_executable(qdax
     main.cpp
     dax.cpp
     tagitem.cpp
     eventworker.cpp
     mainwindow.ui
     mainwindow.cpp
     aboutdialog.ui
     aboutdialog.cpp
)

target_link_libraries(qdax PRIVATE Qt6::Core  Qt6::Gui  Qt6::Widgets dax daxlog)


set_target_properties(qdax PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)