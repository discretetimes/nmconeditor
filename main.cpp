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

int main()
{
    qDBusRegisterMetaType<NMVariantMapMap>();
    QTextStream qout(stdout, QIODevice::WriteOnly);
    QTextStream qin(stdin, QIODevice::ReadOnly);

    // nmcli connection
    // nmcli connection add con-name test3 type 802-3-ethernet ipv4.method manual ipv4.addresses 192.168.1.100/24
    NetworkManager::Connection::List connectionList = NetworkManager::listConnections();

    if (connectionList.isEmpty()) {
        qInfo() << "No network connections are configured.";
    } else {
        qInfo() << "Configured Network Connections (" << connectionList.count() << "):";
        for (const NetworkManager::Connection::Ptr connection : connectionList) {
            if (connection) { // Always good to check if the pointer is valid
                NetworkManager::ConnectionSettings::Ptr settings = connection->settings();

                // NetworkManager::Ipv4Setting::Ptr ipv4Setting = setting.staticCast<NetworkManager::Ipv4Setting>();
                if (settings->connectionType() == NetworkManager::ConnectionSettings::Wired){
                    // if(ipv4Setting->method() == NetworkManager::Ipv4Setting::Manual){
                    //     qInfo().nospace() << "  ID: " << connection->name();
                    // }

                // nmcli connection show test3

                qDebug() << "Wired Settings";

                // if (ipv4Setting) {
                //     // qInfo().nospace() << "    IPv4 Method: " << ipv4Setting->methodToString(ip4Setting->method());
                //     if (ipv4Setting.Method() == NetworkManager::Ipv4Setting::Manual) {
                //         QList<NetworkManager::IpAddress> addresses = ip4Setting->addresses();
                //         if (!addresses.isEmpty()) {
                //             qInfo().nospace() << "      Static IP: " << addresses.first().address() << "/" << addresses.first().prefix();
                //         }
                //     }
                // } else {
                //     qInfo().nospace() << "    IPv4: (No IPv4 settings or disabled)";
                // }
                // qInfo() << ""; // Blank line for readability
                }
            }
        }
    }
}
