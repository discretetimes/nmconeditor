/*
    SPDX-FileCopyrightText: 2012 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <arpa/inet.h>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WiredSetting>
#include <QCoreApplication>

#include <QDBusMetaType>
#include <QTextStream>
#include <QUuid>

#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusPendingReply>

#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>

using namespace NetworkManager;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // List all wired connections
    qDebug() << "Available wired connections:";
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->settings()->connectionType() == NetworkManager::ConnectionSettings::Wired) {
            qDebug() << "-" << conn->name() << "(UUID:" << conn->uuid() << ")";
        }
    }

    // createWiredConnection("test4", "enp3s0f4u1u1");

    void activateWiredConnection(const QString &connectionName, const QString &interfaceName);

    // Example: Activate a connection
    activateWiredConnection("test4", "enp3s0f4u1u1");

    // Example: Create and activate new connection
    return app.exec();
}

void activateWiredConnection(const QString &connectionName, const QString &interfaceName)
{
    NetworkManager::Connection::List connections = NetworkManager::listConnections();

    for (const NetworkManager::Connection::Ptr &connection : connections) {
        if (connection->name() == connectionName) {
            // Activate the connection
            // connection = connectionName;
            // device = interfaceName;
            // specificObject = connection->path();
            // NetworkManager::activateConnection(connection, device, specificObject); // "/" = default device
            qInfo() << "Activating connection:" << connectionName << "interface: " << interfaceName << "Path: " << connection->path();
            NetworkManager::activateConnection(connectionName, interfaceName, connection->path());
            qDebug() << "Activated connection:" << connectionName;
            return;
        }
    }

    qWarning() << "Connection not found:" << connectionName;
}

void deactivateWiredConnection(const QString &connectionName)
{
    for (const NetworkManager::ActiveConnection::Ptr &active : NetworkManager::activeConnections()) {
        if (active->connection()->name() == connectionName) {
            NetworkManager::deactivateConnection(active->path());
            qDebug() << "Deactivated connection:" << connectionName;
            return;
        }
    }

    qWarning() << "No active connection found:" << connectionName;
}

void createWiredConnection(const QString &name, const QString &interfaceName)
{
    NetworkManager::ConnectionSettings settings;
    settings.setId(name);
    settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings.setInterfaceName(interfaceName);

    // Configure as DHCP (default)
    // settings.setIpv4Method(NetworkManager::ConnectionSettings::Automatic);
    // ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // OR set a static IP
    /*
    settings.setIpv4Method(NetworkManager::ConnectionSettings::Manual);
    NetworkManager::Ipv4Setting ipv4;
    ipv4.setAddresses({
        {QHostAddress("192.168.1.100"), 24, QHostAddress("192.168.1.1")}
    });
    ipv4.setDns({QHostAddress("8.8.8.8"), QHostAddress("8.8.4.4")});
    settings.setIpv4(ipv4);
    */

    // Add wired-specific settings
    NetworkManager::WiredSetting wired;
    // settings.addSetting(wired);

    // Add the new connection
    NetworkManager::addConnection(settings.toMap());
    qDebug() << "Created new wired connection:" << name;
}
