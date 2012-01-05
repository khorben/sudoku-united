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

#ifndef TELEPATHYGAMEINFO_H
#define TELEPATHYGAMEINFO_H

#include "../gameinfo.h"

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/ContactManager>

class TelepathyGameInfo : public GameInfo {
    Q_OBJECT
public:
    TelepathyGameInfo(Tp::AccountPtr account, Tp::ContactPtr contact,
                      QObject *parent = 0);

    QString name() const;
    AbstractClient *client() const;

    Tp::AccountPtr account;
    Tp::ContactPtr contact;

    bool operator ==(GameInfo &other) const;
};

class TelepathyGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    TelepathyGameInfoModel(QObject *parent = 0);

private:
    void buildGameInfoList();
    Tp::AccountPtr findAccount(Tp::ContactManagerPtr cm);

    void setAutoRefresh(bool enabled);

    void addContacts(Tp::ConnectionPtr connection);
private slots:
    void onConnectionStatusChanged(Tp::ConnectionStatus status);
    void onConnectionChanged(Tp::ConnectionPtr connection);
    void onAccountManagerReady(Tp::PendingOperation *operation);
    void onCapabilitiesChanged(const Tp::ContactCapabilities &caps);
    void onAllKnownContactsChanged(const Tp::Contacts &contactsAdded,
                                   const Tp::Contacts &contactsRemoved,
                                   const Tp::Channel::GroupMemberChangeDetails &details);
private:
    Tp::AccountManagerPtr accountManager;
};

#endif // TELEPATHYGAMEINFO_H
