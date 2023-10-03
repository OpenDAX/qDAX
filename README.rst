***************************************************************
OpenDAX - An Open Source Data Acquisition and Control Framework
***************************************************************

----------------
Overview
----------------

OpenDAX is a framework that can be used to build automation and control system
applications.  It consists of methods of reading data from automation specific
hardware, storing that data for use by other parts of the system and performing
logic on those systems.

qDAX is a graphical interface to the OpenDAX tag server.

For more information see the website at https://opendax.org

--------------
Installation
--------------

sudo apt install qt6-base-dev

mkdir build
cd build
cmake ..
make
make test
make install
