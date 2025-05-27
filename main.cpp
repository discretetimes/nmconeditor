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


    QString findDeviceByName(const QString & iface);

    void activateWiredConnection(const QString &connectionName, const QString &interfaceName);

    void deactivateWiredConnection(const QString &connectionName);

    void createWiredConnection(const QString &name, const QString &interfaceName);

    void deleteConnection(const QString &name);

    //
    // void inspectConnectionSettings();

    // TODO
    // void modifyIpv4Setting();



    // QString m_devicePath = "/org/freedesktop/NetworkManager/Devices/3";

    // test
    // QString m_devicePath = findDeviceByName("enp7s18u5u3c2");
    // NetworkManager::Device::Ptr targetDevice = NetworkManager::findNetworkInterface(m_devicePath);
    // NetworkManager::Device::Ptr targetDevice = findDeviceByName(interfaceName);
    // The name of the device's data interface when available, becoming ACTIVATED.
    // NetworkManager::Device::Ptr m_device = NetworkManager::findDeviceByIpFace(targetDevice->ipInterfaceName());
    // qInfo() << "Target Device:" << targetDevice->interfaceName();
    // qInfo() << "Target Device:" << m_device->interfaceName();

    // test
    // activateWiredConnection("test3", "enp7s18u5u3c2");

    // test
    // deactivateWiredConnection("test3");


    // inspectConnectionSettings();

    // test
    // modifyIpv4Setting();

    // test
    // createWiredConnection("test4", "enp3s0f4u1u1");

    // test
    deleteConnection("test4");

    // Example: Create and activate new connection
    return app.exec();
}

void listDevices()
{
    QList<NetworkManager::Device::Ptr> devices = NetworkManager::networkInterfaces();

    if (devices.isEmpty()) {
        qDebug() << "No network devices found.";
    }

    foreach (const NetworkManager::Device::Ptr &device, devices) {
        QString devType;

        switch (device->type()) {
        case NetworkManager::Device::Ethernet:
            devType = "Ethernet";
            break;
        case NetworkManager::Device::Wifi:
            devType = "Wi-Fi";
            break;
        case NetworkManager::Device::Generic:
            devType = "Generic";
            break;
        default:
            devType = "Unknown";
            break;
        }

        qDebug() << "----------------------------------------";
        qDebug() << "Name:" << device->interfaceName();
        qDebug() << "Type:" << devType;
        qDebug() << "State:" << device->state();
        qDebug() << "Uni (D-Bus Path):" << device->uni();


        if (device->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice::Ptr wiredDevice =
                qSharedPointerDynamicCast<NetworkManager::WiredDevice>(device);
            if (wiredDevice) {
                qDebug() << "MAC Address:" << wiredDevice->permanentHardwareAddress();
            }
        }
    }
}

QString findDeviceByName(const QString & iface)
{
    QList<NetworkManager::Device::Ptr> devices = NetworkManager::networkInterfaces();

    foreach (const NetworkManager::Device::Ptr &device, devices) {
        if (device->interfaceName() == iface) {
            qInfo() << "D-Bus object path." << device->uni();
            return device->uni(); // This is the D-Bus object path
        }
    }

    return QString();
}

void activateWiredConnection(const QString &connectionName, const QString &interfaceName)
{
    NetworkManager::Connection::List connections = NetworkManager::listConnections();

    // NetworkManager::WiredDevice::List devices = NetworkManager::WiredDevice::List();

    // for (const NetworkManager::WiredDevice::Ptr &device : devices){
    //     if(device->interfaceName() == interfaceName){
    //         NetworkManager::Device::Ptr targetDevice = device;
    //         qInfo() << "Target Device:" << targetDevice->interfaceName();
    //     }

    // }

    // TODO, how to list all devices, and Path to name

    findDeviceByName(interfaceName);
    // QString m_devicePath = "/org/freedesktop/NetworkManager/Devices/3";
    QString m_devicePath = findDeviceByName(interfaceName);
    NetworkManager::Device::Ptr targetDevice = NetworkManager::findNetworkInterface(m_devicePath);
    // NetworkManager::Device::Ptr targetDevice = findDeviceByName(interfaceName);
    // The name of the device's data interface when available, becoming ACTIVATED.
    // NetworkManager::Device::Ptr m_device = NetworkManager::findDeviceByIpFace(targetDevice->ipInterfaceName());
    qInfo() << "Target Device:" << targetDevice->interfaceName();
    // qInfo() << "Target Device:" << m_device->interfaceName();


    for (const NetworkManager::Connection::Ptr &connection : connections) {
        if (connection->name() == connectionName) {
            // Activate the connection
            // connection = connectionName;
            // device = interfaceName;
            // specificObject = connection->path();

            // NetworkManager::activateConnection(connection, device, specificObject); // "/" = default device
            qInfo() << "Activating connection:" << connectionName << "interface: " << interfaceName << "Path: " << connection->path();
            NetworkManager::activateConnection(connection->path(), targetDevice->uni(), "");
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
            qInfo() << "Deactivated connection:" << connectionName;
            return;
        }
    }

    qWarning() << "No active connection found:" << connectionName;
}

void createAutoWiredConnection(const QString &name, const QString &interfaceName)
{
    // NetworkManager::ConnectionSettings settings;

    NetworkManager::ConnectionSettings *m_setting = new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wired);
    const Device::List deviceList = NetworkManager::networkInterfaces();
    WiredDevice::Ptr wiredDevice;

    // We have to find some wired device
    for (Device::Ptr dev : deviceList) {
        if (dev->type() == Device::Ethernet) {
            wiredDevice = qobject_cast<WiredDevice *>(dev);
            break;
        }
    }

    m_setting->setId(name);
    m_setting->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    m_setting->setInterfaceName(interfaceName);

    // IPv4 settings
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_setting->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
    // ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Manual);
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // QList<NetworkManager::IpAddress> ipAddresses;
    // QHostAddress ip("192.168.1.100");
    // // QHostAddress netmask("255.255.255.0");
    // ipAddresses.append(NetworkManager::IpAddress(ip.toIPv4Address());
    // ipv4Setting->setAddresses(ipAddresses);

    // Add IPv4 setting to the connection

    if (m_setting) {
        qDebug() << "Updated IPv4 Method:" << m_setting->name();
    }

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(m_setting->toMap());

    reply.waitForFinished();
    qInfo() << "---Here---";
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        QDBusObjectPath newConnectionPath = reply.value();
        qInfo() << "Connection added successfully. Path:" << newConnectionPath.path();
        // NetworkManager::Connection::Ptr m_conn = NetworkManager::findConnection(newConnectionPath.path());
        // NetworkManager::Connection connection(newConnectionPath);
        // NetworkManager::ConnectionSettings::Ptr newSettings = connection.settings();
        // qDebug() << (*newSettings.data());
    }
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
    // NetworkManager::WiredSetting wired;
    // settings.addSetting(wired); 

    // Add the new connection
    // NetworkManager::addConnection(settings.toMap());
    // qDebug() << "Created new wired connection:" << name;
}

void createWiredConnection(const QString &name, const QString &interfaceName)
{
    // NetworkManager::ConnectionSettings settings;

    NetworkManager::ConnectionSettings::Ptr m_setting =
        NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wired));
    const Device::List deviceList = NetworkManager::networkInterfaces();
    WiredDevice::Ptr wiredDevice;

    // We have to find some wired device
    for (Device::Ptr dev : deviceList) {
        if (dev->type() == Device::Ethernet) {
            wiredDevice = qobject_cast<WiredDevice *>(dev);
            break;
        }
    }

    m_setting->setId(name);
    m_setting->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    m_setting->setInterfaceName(interfaceName);

    // NetworkManager::ConnectionSettings::Ptr settings = m_setting->Ptr();

    // Set the addresses
    // ipv4Setting->setAddresses(ipv4Addresses);


    // Add IPv4 setting to the connection

    // ipv4Setting->toMap();
    // QDBusPendingReply<QDBusObjectPath> reply0 = NetworkManager::addConnection(m_setting->toMap());

    // IPv4 settings
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_setting->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();

    // Need to be initialized
    ipv4Setting->setInitialized(true);
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Manual);
    // m_setting->settings();
    // qInfo() << "Method : " << ipv4Setting->method();
    // ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automati);
    // ip address
    QList<IpAddress> ipv4Addresses;
    IpAddress addr;
    addr.setIp(QHostAddress("192.168.1.150")); // IP address
    addr.setPrefixLength(24); // Subnet mask (255.255.255.0)
    // addr.setGateway(QHostAddress("192.168.1.1")); // Gateway
    ipv4Addresses.append(addr);

    // Optional: Add a second IP address (if needed)
    IpAddress addr2;
    addr2.setIp(QHostAddress("192.168.1.151"));
    addr2.setPrefixLength(24);
    // No gateway for secondary address (typically only the first has a gateway)
    ipv4Addresses.append(addr2);

    ipv4Setting->setAddresses(ipv4Addresses);
    // qInfo() << "Pre ipv4 settings: " << (*ipv4Setting.data());

    // qInfo() << (*m_setting.data());

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(m_setting->toMap());


    reply.waitForFinished();

    qInfo() << "---Here---";
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        QDBusObjectPath newConnectionPath = reply.value();
        qInfo() << "Connection added successfully. Path:" << newConnectionPath.path();
        // NetworkManager::ConnectionSettings::Ptr newSettings = NetworkManager::findConnection(newConnectionPath.path())->settings();
        // qInfo() << " Retrive: "<< (*newSettings.data());
    }
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
    // NetworkManager::WiredSetting wired;
    // settings.addSetting(wired);

    // Add the new connection
    // NetworkManager::addConnection(settings.toMap());
    // qDebug() << "Created new wired connection:" << name;
}


void deleteConnection(const QString &connectionName)
{
    NetworkManager::Connection::List connections = NetworkManager::listConnections();

    for (const NetworkManager::Connection::Ptr &connection : connections) {
        if (connection->name() == connectionName) {
            // Activate the connection
            // connection = connectionName;
            // device = interfaceName;
            // specificObject = connection->path();

            // TODO
            // use uuid to manage connection
            // NetworkManager::findConnectionByUuid ( const QString & uuid )
            // NetworkManager::activateConnection(connection, device, specificObject); // "/" = default device
            qInfo() << "Deleting connection:" << connectionName;
            connection->remove();
            qDebug() << "Activated connection:" << connectionName;
            return;
        }
    }

    qWarning() << "Connection not found:" << connectionName;
}

void inspectConnectionSettings() {
    // Create or load a connection settings object (e.g., for a wired connection)
    // ConnectionSettings::Ptr connSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wired));

    // // Configure an IPv4 setting and add it
    // Ipv4Setting::Ptr ipv4Setting = Ipv4Setting::Ptr(new Ipv4Setting());
    // ipv4Setting->setMethod(Ipv4Setting::Manual);

    // QList<NetworkManager::IpAddress> list;
    // NetworkManager::IpAddress address;

    // // QList<QMap<QString, QVariant>> addressDataList;

    // // QMap<QString, QVariant> address1Data;

    // // address1Data["address"] = "192.168.1.100";
    // // address1Data["prefix"] = 24; // equivalent to 255.255.255.0

    // // addressDataList.append(address1Data);

    // // ipv4Setting->setAddressData(addressDataList);

    // // address.setNetmask(QHostAddress(d->model.item(i, 1)->text()));
    // // QHostAddress ip("192.168.0.101");
    // QHostAddress netmask("255.255.255.0");
    // address.setIp(QHostAddress("192.168.0.101"));
    // address.setNetmask(netmask);
    // list.append(address);

    // ipv4Setting->setAddresses(list);

    // // connSettings->setting();

    // // Retrieve the IPv4 setting
    // Setting::Ptr retrievedSetting = connSettings->setting(Setting::Ipv4);

    // // Check if the setting exists and cast to Ipv4Setting
    // if (retrievedSetting && retrievedSetting->type() == Setting::Ipv4) {
    //     Ipv4Setting::Ptr ipv4 = retrievedSetting.dynamicCast<Ipv4Setting>();
    //     qDebug() << "IPv4 Method:" << ipv4->method(); // e.g., "manual"
    //     qDebug() << "IPv4 Addresses:" << ipv4->addresses();
    // } else {
    //     qDebug() << "No IPv4 setting found.";
    // }
}

void modifyIpv4Setting() {
    // Create and configure a ConnectionSettings object

    QString connectionName = "test3";

    NetworkManager::Connection::List connections = NetworkManager::listConnections();

    for (const NetworkManager::Connection::Ptr &connection : connections) {

        if (connection->name() == connectionName) {

            // NetworkManager::activateConnection(connection, device, specificObject); // "/" = default device
            qInfo() << "Modify connection:" << connectionName << "Path: " << connection->path();

            // Retrieve the Ipv4Setting
            NetworkManager::ConnectionSettings::Ptr m_conn = connection->settings();

            // Modify ipv4 setting
            NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_conn->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
            ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);
            m_conn->settings();
            qDebug() << "Modifying connection:" << ipv4Setting->method();

            QDBusPendingReply<> reply = connection->update(m_conn->toMap());

            reply.waitForFinished();
            qInfo() << "---Here---";
            if (reply.isError()) {
                qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
            }

            // NetworkManager::reloadConnections();
            // NetworkManager::Setting::Ptr m_ipv4 = m_conn->id();
            // NetworkManager::Ip4Setting::Ptr ipv4Setting;

            // if (m_ipv4) {
            //     ipv4Setting = m_ipv4.dynamicCast<NetworkManager::Ipv4Setting>();
            // }

            // qInfo() << "Current DNS servers:" << m_ipv4->method();

            qDebug() << "Modified connection:" << connectionName;
            return;
        }
    }
}


