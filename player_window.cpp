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
#include "player_window.h"
#include <QGst/enums.h>
#include <QString>

static int clockTimeToInt(quint64 time)
{
  return int(time >> 32);
}

static quint64 clockTimeFromInt(int time)
{
  return quint64(time) << 32;
}


class SeekingSlider : public QSlider
{
};

PlayerWindow::PlayerWindow(Player * player)
{
  m_player = player;
  createUi();
}

void PlayerWindow::createUi()
{
  vbox = new QVBoxLayout ();
  hbox = new QHBoxLayout ();
  vbox->addLayout(hbox);
  setLayout(vbox);
  
  play_pause = new QPushButton("Play");
  hbox->addWidget(play_pause);

  hbox->addStretch(0);

  info = new QLabel();
  hbox->addWidget(info);
  
  position = new SeekingSlider();
  position->setTracking(false);
  position->setOrientation(Qt::Horizontal);
  position->setMinimum(0);
  position->setMaximum(0);
  vbox->addWidget(position);
  
  resize(320, 75);
  show();

  connect(m_player, SIGNAL(durationChanged(quint64)),
	  this, SLOT(durationChanged(quint64)));
  connect(m_player, SIGNAL(stateChanged(QGst::State)),
	  this, SLOT(stateChanged(QGst::State)));
  connect(m_player, SIGNAL(positionChanged(quint64)),
	  this, SLOT(positionChanged(quint64)));
  connect(play_pause, SIGNAL(clicked(bool)),
	  this, SLOT(playButtonClicked(bool)));
  connect(position, SIGNAL(sliderMoved(int)),
	  this, SLOT(sliderMoved(int)));

  durationChanged(-1);
  positionChanged(-1);
}

void PlayerWindow::stateChanged(QGst::State state)
{
  qDebug() << "State changed" << state;
  switch (state) {
  case QGst::StatePlaying:
    play_pause->setText("Pause");
    break;
  default:
    play_pause->setText("Play");
    break;
  }
}

void PlayerWindow::positionChanged(quint64 pos)
{
  position->setSliderPosition(clockTimeToInt(pos));
  cur_pos = pos;
  updateDisplay();
}

void PlayerWindow::updateDisplay() {
  QString pos_str ("--:--:--");
  QString dur_str ("--:--:--");
  
  if (cur_pos != -1) {
    pos_str = QGst::Clock::timeFromClockTime(cur_pos).toString();
  }

  if (duration != -1) {
    dur_str = QGst::Clock::timeFromClockTime(duration).toString();
  }

  info->setText(pos_str + " / " + dur_str);
}

void PlayerWindow::durationChanged(quint64 duration)
{
  qDebug() << "Duration changed" <<
    QGst::Clock::timeFromClockTime(duration);
  // should use QGst::ClockTimeNone, but it was not present in the library
  if (!((duration == -1) || (duration == 0))) {
    position->setMaximum(clockTimeToInt(duration));
  }
  this->duration = duration;
  updateDisplay();
}

void PlayerWindow::playButtonClicked(bool checked)
{
  qDebug() << "Playing pipeline";
  if (m_player->state() == QGst::StatePlaying) {
    m_player->pause();
  } else {
    m_player->play();
  }
}

void PlayerWindow::sliderMoved(int value)
{
  m_player->setPosition(clockTimeFromInt(value));
}
