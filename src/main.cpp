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

 * Main source code file for the Qt Gui Interface Module
 */

#include <QApplication>
#include <QPushButton>
#include "mainwindow.h"
#include "dax.h"

Dax dax("qdax");

int
main(int argc, char *argv[])
{
    dax_state *ds;
    int retval;

    QApplication app(argc, argv);

    dax.configure(argc, argv, CFG_CMDLINE);

    MainWindow mainwindow;

    mainwindow.show();

    retval = app.exec(); // .exec starts QApplication and related GUI, this line starts 'event loop'
    return retval;
}