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

using namespace NetworkManager;

int main()
{
    qDBusRegisterMetaType<NMVariantMapMap>();
    QTextStream qout(stdout, QIODevice::WriteOnly);
    QTextStream qin(stdin, QIODevice::ReadOnly);

    //NetworkManager::ConnectionSettings::Ptr connectionSettings;
    // NetworkManager::WiredSetting::Ptr settings = connectionSettings->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
    NetworkManager::ConnectionSettings *settings = new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wired);
    const Device::List deviceList = NetworkManager::networkInterfaces();

    WiredDevice::Ptr wiredDevice;

    // We have to find some wired device
    for (Device::Ptr dev : deviceList) {
        if (dev->type() == Device::Ethernet) {
            wiredDevice = qobject_cast<WiredDevice *>(dev);
            qInfo() << "Device : " << wiredDevice->interfaceName();
            break;
        }
    }

    if (!wiredDevice) {
        return 1;
    }

    // NetworkManager::ConnectionSettings
    // Now we will prepare our new connection, we have to specify ID and create new UUID
    QString wiredID = "test";

    settings->setId(wiredID);
    settings->setUuid(QUuid::createUuid().toString().mid(1, QUuid::createUuid().toString().length() - 2));
    settings->setConnectionType(NetworkManager::ConnectionSettings::Wired);
    settings->setAutoconnect(true);

    // NetworkManager::WiredSetting

    // NetworkManager::Ipv4Setting
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    QString ConfigMethod = "Manual";
    // ConfigMethod = "Automatic";
    if(ConfigMethod == "Automatic")
        ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);
    else{
        qInfo() << "set static ip addr";
        ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Manual);

        QList<NetworkManager::IpAddress> list;
        NetworkManager::IpAddress address;

        // address.setNetmask(QHostAddress(d->model.item(i, 1)->text()));
        QHostAddress ip("192.168.0.101");
        QHostAddress netmask("255.255.255.0");
        address.setIp(ip);
        address.setNetmask(netmask);
        list.append(address);

        // ipv4Setting->setAddresses(list);
        // address.setPrefixLength(24);
        // ip4Setting->setGateway("192.168.1.1");
        // ip4Setting->addDns("8.8.8.8");
    }
    qDebug() << (*ipv4Setting.data());
    // We try to add and activate our new wireless connection
    // settings::ConnectionSettings (ipv4Setting);
    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(settings->toMap());

    // auto watcher = new QDBusPendingCallWatcher(reply, this);
    // watcher->setProperty("action", AddConnection);
    // watcher->setProperty("connection", map.value(QStringLiteral("connection")).value(QStringLiteral("id")));


    reply.waitForFinished();
    qInfo() << "---Here---";
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        QDBusObjectPath newConnectionPath = reply.value();
        qInfo() << "Connection added successfully. Path:" << newConnectionPath.path();
    }

    //NetworkManager::Connection connection(reply.value());
   // NetworkManager::ConnectionSettings::Ptr newSettings = connection.settings();
    // qDebug() << (*newSettings.data());


    // Check if this connection was added successfuly
    // if (reply.isValid()) {
        // Now our connection should be added in NetworkManager and we can print all settings pre-filled from NetworkManager
        // qInfo() << "---Here---" << connectionPath;
        // NetworkManager::Connection connection(reply);
        // NetworkManager::ConnectionSettings::Ptr newSettings = connection.settings();
        // Print resulting settings
        // qInfo() << (*newSettings.data());

    // } else {
        // qInfo() << "Connection failed: " << reply.error();
    // }
}
