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

#include "telepathygameinfo.h"
#include "telepathyclient.h"

#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/ContactCapabilities>

TelepathyGameInfo::TelepathyGameInfo(Tp::AccountPtr account,
                                     Tp::ContactPtr contact,
                                     QObject *parent) :
    GameInfo(parent), account(account), contact(contact) {

}

QString TelepathyGameInfo::name() const {
    return contact->alias();
}

AbstractClient *TelepathyGameInfo::client() const {
    return new TelepathyClient();
}

bool TelepathyGameInfo::operator ==(const GameInfo &other) const {
    const TelepathyGameInfo *otherGameInfo =
            qobject_cast<const TelepathyGameInfo *>(&other);

    if (!otherGameInfo)
        return false;

    return contact->id() == otherGameInfo->contact->id();
}

TelepathyGameInfoModel::TelepathyGameInfoModel(QObject *parent) :
    GameInfoModel(parent) {

    accountManager = Tp::AccountManager::create(Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                           Tp::Account::FeatureCore),
                                                Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                              Tp::Connection::FeatureRoster),
                                                Tp::ChannelFactory::create(QDBusConnection::sessionBus()),
                                                Tp::ContactFactory::create(Tp::Contact::FeatureCapabilities | Tp::Contact::FeatureAlias | Tp::Contact::FeatureSimplePresence));

    if (!accountManager->isReady())
        connect(accountManager->becomeReady(),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onAccountManagerReady(Tp::PendingOperation *)));
    else
        buildGameInfoList();
}

void TelepathyGameInfoModel::onAccountManagerReady(Tp::PendingOperation *operation) {
    if (operation->isError()) {
        qWarning() << "Failed to load account manager: " << operation->errorMessage();
        setState(Complete);
        return;
    }

    buildGameInfoList();
}

void TelepathyGameInfoModel::buildGameInfoList() {
    if (rowCount(QModelIndex())) {
        beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1);
        m_gameInfoList.clear();
        endRemoveRows();
    }

    foreach (Tp::AccountPtr account, accountManager->allAccounts()) {
        if (!account->isValid()) {
            qDebug() << "Account not valid...";
            continue;
        }

        connect(account.data(), SIGNAL(connectionChanged(Tp::ConnectionPtr)),
                SLOT(onConnectionChanged(Tp::ConnectionPtr)));

        addContacts(account->connection());
    }

    setState(Complete);
}

void TelepathyGameInfoModel::onConnectionChanged(Tp::ConnectionPtr connection) {
    Q_UNUSED(connection);

    Tp::AccountPtr account(qobject_cast<Tp::Account *>(sender()));
    Q_ASSERT(account);

    if (!connection)
        return;

    connect(connection.data(), SIGNAL(statusChanged(Tp::ConnectionStatus)),
            SLOT(onConnectionStatusChanged(Tp::ConnectionStatus)));

    if (connection->status() == Tp::ConnectionStatusConnected)
        addContacts(connection);
}

void TelepathyGameInfoModel::onConnectionStatusChanged(Tp::ConnectionStatus status) {
    Q_UNUSED(status);

    Tp::ConnectionPtr connection(qobject_cast<Tp::Connection *>(sender()));
    Q_ASSERT(connection);

    addContacts(connection);
}

void TelepathyGameInfoModel::addContacts(Tp::ConnectionPtr connection) {
    if (!connection || connection->status() != Tp::ConnectionStatusConnected)
        return;

    if (!connection->capabilities().streamTubes())
        return;

    Tp::ContactManagerPtr contactManager = connection->contactManager();
    Tp::AccountPtr account = findAccount(contactManager);

    if (!account)
        return;

    connect(contactManager.data(),
            SIGNAL(allKnownContactsChanged(Tp::Contacts,Tp::Contacts,Tp::Channel::GroupMemberChangeDetails)),
            SLOT(onAllKnownContactsChanged(Tp::Contacts,Tp::Contacts,Tp::Channel::GroupMemberChangeDetails)));

    Tp::Contacts contacts =
            contactManager->allKnownContacts();

    foreach (Tp::ContactPtr contact, contacts.toList()) {
        connect(contact.data(),
                SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                SLOT(onCapabilitiesChanged(Tp::ContactCapabilities)));

        qDebug() << contact->id()
                 << contact->capabilities().streamTubeServices()
                 << contact->alias();

        if (contact->capabilities().streamTubes("x-sudoku-united-game")) {
            TelepathyGameInfo *gameInfo = new TelepathyGameInfo(account,
                                                                contact,
                                                                this);
            insertGameInfo(gameInfo);
        }
    }
}

Tp::AccountPtr TelepathyGameInfoModel::findAccount(Tp::ContactManagerPtr cm) {
    if (!cm || !accountManager->isReady())
        return Tp::AccountPtr();

    foreach (Tp::AccountPtr account, accountManager->allAccounts()) {
        if (!account->connection())
            continue;

        if (account->connection()->contactManager() == cm)
            return account;
    }

    return Tp::AccountPtr();
}

void TelepathyGameInfoModel::onCapabilitiesChanged(const Tp::ContactCapabilities &caps) {
    Q_UNUSED(caps)

    Tp::ContactPtr contact((Tp::Contact *) sender());
    Tp::AccountPtr account = findAccount(contact->manager());

    if (!account) {
        qWarning() << "Could not find account for contact manager "
                   << contact->manager();
        return;
    }

    if (!contact->capabilities().streamTubes("x-sudoku-united-game")) {

        int i = 0;
        foreach (GameInfo *gameInfo, m_gameInfoList) {
            TelepathyGameInfo *info = (TelepathyGameInfo *) gameInfo;

            if (info->contact->id() == contact->id() &&
                    info->account->objectPath() == account->objectPath()) {

                beginRemoveRows(QModelIndex(), i, i);
                m_gameInfoList.removeAt(i);
                endRemoveRows();

                break;
            }

            i++;
        }

        return;
    }

    TelepathyGameInfo *gameInfo = new TelepathyGameInfo(account,
                                                        contact,
                                                        this);
    insertGameInfo(gameInfo);
}


void TelepathyGameInfoModel::onAllKnownContactsChanged(const Tp::Contacts &contactsAdded,
                                                       const Tp::Contacts &contactsRemoved,
                                                       const Tp::Channel::GroupMemberChangeDetails &details) {
    Q_UNUSED(details)

    Tp::ContactManagerPtr contactManager((Tp::ContactManager *) sender());
    Tp::AccountPtr account = findAccount(contactManager);

    if (!account) {
        qWarning() << "Could not find account for contact manager "
                   << contactManager;
        return;
    }

    foreach (Tp::ContactPtr contact, contactsAdded) {
        connect(contact.data(),
                SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                SLOT(onCapabilitiesChanged(Tp::ContactCapabilities)));

        if (contact->capabilities().streamTubes("x-sudoku-united-game")) {
            TelepathyGameInfo *gameInfo = new TelepathyGameInfo(account,
                                                                contact,
                                                                this);
            insertGameInfo(gameInfo);
        }
    }

    foreach (Tp::ContactPtr contact, contactsRemoved) {
        disconnect(contact.data(),
                   SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                   this, SLOT(onCapabilitiesChanged(Tp::ContactCapabilities)));

        int i = 0;
        foreach (GameInfo *gameInfo, m_gameInfoList) {
            TelepathyGameInfo *info = (TelepathyGameInfo *) gameInfo;

            if (info->contact->id() == contact->id() &&
                    info->account->objectPath() == account->objectPath()) {

                beginRemoveRows(QModelIndex(), i, i);
                m_gameInfoList.removeAt(i);
                endRemoveRows();

                break;
            }

            i++;
        }
    }
}

void TelepathyGameInfoModel::setAutoRefresh(bool enabled) {
    GameInfoModel::setAutoRefresh(enabled);

    if (!enabled) {
        disconnect(this, SLOT(onConnectionStatusChanged(Tp::ConnectionStatus)));
        disconnect(this, SLOT(onConnectionChanged(Tp::ConnectionPtr)));
        disconnect(this, SLOT(onCapabilitiesChanged(Tp::ContactCapabilities)));
        disconnect(this,
                   SLOT(onAllKnownContactsChanged(Tp::Contacts,Tp::Contacts,Tp::Channel::GroupMemberChangeDetails)));
    } else {
        buildGameInfoList();
    }
}
