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

#include "ui_mainwindow.h"
#include "dax.h"
#include "tagList.h"


class MainWindow : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

    private:
        Dax& dax;
        TagList *_taglist;

    public:
        explicit MainWindow(Dax& d, QWidget *parent = nullptr);
        ~MainWindow();

    public slots:
        void connect(void);
        void disconnect(void);
};

