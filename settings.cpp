/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.h"
#include "game.h"

Settings::Settings(QObject *parent) :
    QSettings("Bithub", "Sudoku United", parent),
    m_hapticFeedbackEnabled(true),
    m_bluetoothEnabled(true),
    m_lastGame(NULL)
{
    loadSettings();
}

void Settings::loadSettings() {
    setPlayerName(value("playerName", "Player").toString());
    setHapticFeedbackEnabled(value("hapticFeedbackEnabled", true).toBool());
    setBluetoothEnabled(value("bluetoothEnabled", true).toBool());

    QVariant lastGameVariant = value("lastGame");
    if (!lastGameVariant.isNull()) {
        QDataStream gameStream(lastGameVariant.toByteArray());

        m_lastGame = new Game(this);
        gameStream >> *m_lastGame;

        if (gameStream.status() != QDataStream::Ok) {
            delete m_lastGame;
            m_lastGame = NULL;
        }
    } else {
        m_lastGame = NULL;
    }
}

void Settings::saveSettings() {
    setValue("playerName", playerName());
    setValue("hapticFeedbackEnabled", hapticFeedbackEnabled());
    setValue("bluetoothEnabled", bluetoothEnabled());
    if (m_lastGame) {
        QByteArray buffer;
        QDataStream gameStream(&buffer, QIODevice::WriteOnly);
        gameStream << *m_lastGame;

        setValue("lastGame", buffer);
    } else {
        remove("lastGame");
    }

    sync();
}

QString Settings::playerName() const {
    return m_playerName;
}

void Settings::setPlayerName(const QString &playerName) {
    if (playerName == m_playerName)
        return;

    m_playerName = playerName;

    emit playerNameChanged();
}

bool Settings::hapticFeedbackEnabled() const {
    return m_hapticFeedbackEnabled;
}

void Settings::setHapticFeedbackEnabled(bool enableHapticFeedback) {
    if (m_hapticFeedbackEnabled == enableHapticFeedback)
        return;

    m_hapticFeedbackEnabled = enableHapticFeedback;

    emit hapticFeedbackEnabled();
}

bool Settings::bluetoothEnabled() const {
    return m_bluetoothEnabled;
}

void Settings::setBluetoothEnabled(bool enableBluetooth) {
    if (m_bluetoothEnabled == enableBluetooth)
        return;

    m_bluetoothEnabled = enableBluetooth;

    emit bluetoothEnabledChanged();
}

Game *Settings::lastGame() const {
    return m_lastGame;
}

void Settings::setLastGame(Game *game) {
    m_lastGame = game;
}
