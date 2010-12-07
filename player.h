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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <QGst/Global>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/GhostPad>
#include <QGst/Structure>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Clock>
#include <QGst/Event>
#include <QGst/Element>
#include <QGlib/Signal>
#include <QMetaType>
#include <QtGlobal>
#include <QTimer>

Q_DECLARE_METATYPE(QGst::State)

class Player : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QGst::State state READ state WRITE setState NOTIFY stateChanged)
	Q_PROPERTY(quint64 position READ position NOTIFY positionChanged)
	Q_PROPERTY(quint64 duration READ duration NOTIFY duration)
public:
    Player(const QString & fileName);
    ~Player();
    
    void setState(QGst::State state);
    QGst::State state();
    quint64 position();
    quint64 duration();

    void play(void);
    void pause(void);

    private Q_SLOTS:
    void queryPositionDuration();

Q_SIGNALS:
    void stateChanged(QGst::State);
    void positionChanged(quint64);
    void durationChanged(quint64);

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void onNewDecodedPad(QGst::PadPtr newPad);
    void privSetState(QGst::State state);
    void privSetPosition(quint64 position);
    void privSetDuration(quint64 duration);
    
    static QGst::BinPtr createAudioSinkBin();
    QGst::State m_state;
    quint64 m_duration;
    quint64 m_position;
    QGst::PipelinePtr m_pipeline;
    QTimer *position_timer;
};

#endif
