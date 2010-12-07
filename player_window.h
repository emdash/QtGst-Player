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

#ifndef __PLAYER_WINDOW_H__
#define __PLAYER_WINDOW_H__

#include <QtGui>
#include <QGst/Global>
#include <QtGlobal>
#include <QMetaType>
#include "player.h"

class PlayerWindow : QWidget
{
    Q_OBJECT
 public:
  
    PlayerWindow(Player * player);
    
    public Q_SLOTS:
    void stateChanged(QGst::State state);
    void positionChanged(quint64 position);
    void durationChanged(quint64 position);
    void playButtonClicked(bool checked);
    
 private:
    void createUi();
    void updateDisplay();
    
    Player *m_player;
    QHBoxLayout *hbox;
    QVBoxLayout *vbox;
    QPushButton *play_pause;
    QSlider *position;
    QLabel *info;
    quint64 cur_pos;
    quint64 duration;
};

#endif
