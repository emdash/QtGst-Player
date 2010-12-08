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

#include "player.h"
#include <QtCore>
#include <QObject>

Player::Player(const QString & fileName)
{
    m_pipeline = QGst::Pipeline::create();

    QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc");
    QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin2");

    filesrc->setProperty("location", fileName);
    QGlib::Signal::connect(decodebin, "new-decoded-pad", this, &Player::onNewDecodedPad);

    m_pipeline->add(filesrc);
    m_pipeline->add(decodebin);
    filesrc->link(decodebin);

    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::Signal::connect(bus, "message", this, &Player::onBusMessage);

    m_state = QGst::StateNull;
    m_position = 0L;
    m_duration = 0L;

    position_timer = new QTimer(this);
    connect(position_timer, SIGNAL(timeout()), this, SLOT(queryPositionDuration()));
}

Player::~Player()
{
    m_pipeline->setState(QGst::StateNull);

    /* When m_pipeline is destructed, the last reference to our pipeline will be gone,
     * and with it all the elements, buses, etc will be destroyed too. As a result,
     * there is no need to cleanup here. */
}

QGst::State Player::state()
{
  return m_state;
}

void Player::setState(QGst::State value)
{
    m_pipeline->setState(value);
}

void Player::privSetState(QGst::State value)
{
    if (value != m_state) {
	m_state = value;
	Q_EMIT stateChanged(value);
	if (value == QGst::StatePlaying) {
	  position_timer->start(500);
	} else {
	  position_timer->stop();
	}
    }
}

quint64 Player::duration()
{
    return m_duration;
}

void Player::privSetDuration(quint64 value)
{
    if (value != m_duration) {
	m_duration = value;
	Q_EMIT durationChanged(value);
    }
}

quint64 Player::position()
{
    return m_position;
}

void Player::setPosition(quint64 value)
{
  m_pipeline->seek(QGst::FormatTime, QGst::SeekFlagFlush | QGst::SeekFlagAccurate, value);
}

void Player::privSetPosition(quint64 value)
{
  if (value != m_position) {
    m_position = value;
    Q_EMIT positionChanged(value);
  }
}

void Player::queryPositionDuration()
{
  QGst::PositionQueryPtr pos_query = QGst::PositionQuery::create(QGst::FormatTime);
  m_pipeline->query(pos_query);
  privSetPosition(pos_query->position());
  
  QGst::DurationQueryPtr dur_query = QGst::DurationQuery::create(QGst::FormatTime);
  m_pipeline->query(dur_query);
  privSetDuration(dur_query->duration());
}

void Player::play(void)
{
    setState(QGst::StatePlaying); 
}

void Player::pause(void)
{
    setState(QGst::StatePaused);
}

void Player::onBusMessage(const QGst::MessagePtr & message)
{
    switch(message->type()) {
    case QGst::MessageEos:
    case QGst::MessageError:
	//Some error occurred.
        /* QCoreApplication::quit is safe to be called in another thread.
         * It will schedule the main event loop to exit, when execution
         * in the main thread has reached the event loop. */
        QCoreApplication::quit();
        break;
    case QGst::MessageStateChanged:
	if (message->source() == m_pipeline) {
	    privSetState(message.dynamicCast<QGst::StateChangedMessage>
			 ()->newState());
	}
	break;
    default:
        break;
    }
}

/* This method will be called every time a new "src" pad is available on the decodebin2 element.
 * Here we have to check what kind of data this pad transfers (usually it is either "audio/x-raw-*"
 * or "video/x-raw-*") and connect an appropriate sink that can handle this type of data. */
void Player::onNewDecodedPad(QGst::PadPtr newPad)
{

    QGst::CapsPtr caps = newPad->caps();
    QGst::SharedStructure structure = caps->structure(0);

    /* The caps' first structure's name tells us what kind of data the pad transfers.
     * Here we want to handle either audio/x-raw-int or audio/x-raw-float. Both types
     * can be handled by the audioconvert element that is contained in the audioSinkBin,
     * so there is no need to handle them separately */
    if (structure.name().contains("audio/x-raw")) {
        QGst::BinPtr audioSinkBin = createAudioSinkBin();
        m_pipeline->add(audioSinkBin);

        /* The newly created bin must go to the playing state in order to function.
         * Here we tell it to synchronise its state with its parent, the pipeline,
         * which is scheduled to go to the playing state. */
        audioSinkBin->syncStateWithParent();

        newPad->link(audioSinkBin->getStaticPad("sink"));
    }
}

QGst::BinPtr Player::createAudioSinkBin()
{
    QGst::BinPtr bin = QGst::Bin::create();

    QGst::ElementPtr audioconvert = QGst::ElementFactory::make("audioconvert");
    QGst::ElementPtr audiosink = QGst::ElementFactory::make("autoaudiosink");

    bin->add(audioconvert);
    bin->add(audiosink);
    audioconvert->link(audiosink);

    /* Add a sink pad to the bin that proxies the sink pad of the audioconvert element */
    bin->addPad(QGst::GhostPad::create(audioconvert->getStaticPad("sink"), "sink"));

    return bin;
}
