/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010  Brandon Lewis <brandon.lews@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <signal.h>
#include <QtCore>
#include "player.h"
#include "player_window.h"

static void sighandler(int code)
{
    Q_UNUSED(code);
    QCoreApplication::quit();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QString fileName;
    if (argc > 1) {
        fileName = QFile::decodeName(argv[1]);
    }

    if (!QFile::exists(fileName)) {
        std::cerr << "Usage: " << argv[0] << " fileToPlay" << std::endl;
        return 1;
    }

    Player *p = new Player(fileName);
    PlayerWindow *w = new PlayerWindow(p);

    signal(SIGINT, sighandler);
    int result = app.exec();

    delete p; // we must delete all gstreamer objects before calling QGst::cleanup()
    QGst::cleanup();

    return result;
}
