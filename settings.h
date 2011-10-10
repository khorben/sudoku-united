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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QtDeclarative>

class Game;
class HighscoreModel;

class Settings : public QSettings
{
    Q_OBJECT
    Q_ENUMS(QuickAccessAction)
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName
               NOTIFY playerNameChanged)
    Q_PROPERTY(bool hapticFeedbackEnabled READ hapticFeedbackEnabled
               WRITE setHapticFeedbackEnabled
               NOTIFY hapticFeedbackEnabledChanged)
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled
               WRITE setBluetoothEnabled
               NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(Game *lastGame READ lastGame WRITE setLastGame)
    Q_PROPERTY(bool showGameTimer READ showGameTimer WRITE setShowGameTimer
               NOTIFY showGameTimerChanged)
    Q_PROPERTY(QuickAccessAction quickAccessAction READ quickAccessAction
               WRITE setQuickAccessAction NOTIFY quickAccessActionChanged)
public:
    enum QuickAccessAction {
        UndoAction,
        HintAction
    };

public:
    explicit Settings(QObject *parent = 0);

    QString playerName() const;
    void setPlayerName(const QString &playerName);

    bool hapticFeedbackEnabled() const;
    void setHapticFeedbackEnabled(bool enableHapticFeedback);

    bool bluetoothEnabled() const;
    void setBluetoothEnabled(bool enableBluetooth);

    Game *lastGame() const;
    void setLastGame(Game *game);

    bool showGameTimer() const;
    void setShowGameTimer(bool shown);

    HighscoreModel *highscoreModel() const;
    void setHighscoreModel(HighscoreModel *highscoreModel);

    QuickAccessAction quickAccessAction() const;
    void setQuickAccessAction(QuickAccessAction action);

    Q_INVOKABLE
    void saveSettings();
signals:
    void playerNameChanged();
    void hapticFeedbackEnabledChanged();
    void bluetoothEnabledChanged();
    void showGameTimerChanged();
    void quickAccessActionChanged();
public slots:

private:
    void loadSettings();
private:
    QString m_playerName;
    bool m_hapticFeedbackEnabled;
    bool m_bluetoothEnabled;
    Game *m_lastGame;
    bool m_showGameTimer;
    HighscoreModel *m_highscoreModel;
    QuickAccessAction m_quickAccessAction;
};

QML_DECLARE_TYPE(Settings)

#endif // SETTINGS_H
