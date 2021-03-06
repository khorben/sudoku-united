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
    Q_ENUMS(QuickAccessAction LongPressAction)
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
    Q_PROPERTY(bool showedJoinHelp READ showedJoinHelp WRITE setShowedJoinHelp NOTIFY showedJoinHelpChanged)
    Q_PROPERTY(LongPressAction longPressAction READ longPressAction WRITE setLongPressAction NOTIFY longPressActionChanged)
public:
    enum QuickAccessAction {
        UndoAction,
        HintAction
    };

    enum LongPressAction {
        IgnoreAction,
        InsertNote
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

    QUuid playerUuid() const;

    bool showedJoinHelp() const;
    void setShowedJoinHelp(bool showed);

    LongPressAction longPressAction() const;
    void setLongPressAction(LongPressAction action);

    Q_INVOKABLE
    void saveSettings();
signals:
    void playerNameChanged();
    void hapticFeedbackEnabledChanged();
    void bluetoothEnabledChanged();
    void showGameTimerChanged();
    void quickAccessActionChanged();
    void showedJoinHelpChanged();
    void longPressActionChanged();
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
    QUuid m_playerUuid;
    bool m_showedJoinHelp;
    LongPressAction m_longPressAction;
};

QML_DECLARE_TYPE(Settings)
Q_DECLARE_METATYPE(QUuid)

#endif // SETTINGS_H
