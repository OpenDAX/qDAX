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

 *  Source code file for main window class
 */

#include "aboutdialog.h"
#include "dax.h"
#include <config.h>

extern Dax dax;

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    textEdit->setReadOnly(true);
    QString html("<p><b>qDax</b></p>\n");
    html += "<p>Version: ";
    html += VERSION;
    html += "</p>\n";
    html += "<p>Compiled: ";
    html += __DATE__;
    html += " - ";
    html += __TIME__;
    html += "</p>\n";
    html += "<p>Copyright (c) 2023 Phil Birkelbach<br>\n";
    html += "Licensed under the GPL v2.0</p>\n";

    textEdit->setHtml(html);
}

AboutDialog::~AboutDialog() {
    ;
}


