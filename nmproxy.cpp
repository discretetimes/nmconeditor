#include "nmproxy.h"

#include <QDebug>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/ActiveConnection>
#include <QDBusPendingReply>

using namespace NetworkManager;

NMProxy::NMProxy(QObject *parent) : QObject(parent)
{
}

void NMProxy::listDevices()
{
    QList<NetworkManager::Device::Ptr> devices = NetworkManager::networkInterfaces();
    if (devices.isEmpty()) {
        qInfo() << "No network devices found.";
    }
    for (const auto &device : devices) {
        qInfo() << "Name:" << device->interfaceName() << "Type:" << device->type() << "State:" << device->state();
    }
}

QStringList NMProxy::listWiredConnections()
{
    QStringList connectionNames;
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->settings()->connectionType() == NetworkManager::ConnectionSettings::Wired) {
            connectionNames.append(conn->name());
        }
    }
    return connectionNames;
}

QString NMProxy::findDeviceByName(const QString &iface)
{
    for (const auto &device : NetworkManager::networkInterfaces()) {
        if (device->interfaceName() == iface) {
            return device->uni();
        }
    }
    return QString();
}

void NMProxy::activateWiredConnection(const QString &connectionName, const QString &interfaceName)
{
    Connection::Ptr connectionToActivate;
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->name() == connectionName) {
            connectionToActivate = conn;
            break;
        }
    }

    if (!connectionToActivate) {
        qWarning() << "Connection" << connectionName << "not found.";
        return;
    }

    QString devicePath = findDeviceByName(interfaceName);
    if (devicePath.isEmpty()) {
        qInfo() << "Device" << interfaceName << "not found.";
        return;
    }

    NetworkManager::activateConnection(connectionToActivate->path(), devicePath, "/");
}

void NMProxy::deactivateWiredConnection(const QString &connectionName)
{
    for (const auto &active : NetworkManager::activeConnections()) {
        if (active->connection()->name() == connectionName) {
            NetworkManager::deactivateConnection(active->path());
            qInfo() << "Deactivated connection:" << connectionName;
            return;
        }
    }
    qWarning() << "No active connection found with name:" << connectionName;
}

void NMProxy::createAutoWiredConnection(const QString &name, const QString &interfaceName)
{
    ConnectionSettings settings(ConnectionSettings::Wired);
    settings.setId(name);
    settings.setUuid(ConnectionSettings::createNewUuid());
    settings.setInterfaceName(interfaceName);

    Ipv4Setting::Ptr ipv4Setting = settings.setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(Ipv4Setting::Automatic);

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(settings.toMap());
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        qInfo() << "Connection added successfully. Path:" << reply.value().path();
    }
}

void NMProxy::createWiredConnection(const QString &name, const QString &interfaceName)
{
    ConnectionSettings::Ptr settings =
        ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wired));
    settings->setId(name);
    settings->setUuid(ConnectionSettings::createNewUuid());
    settings->setInterfaceName(interfaceName);

    Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setInitialized(true);
    ipv4Setting->setMethod(Ipv4Setting::Manual);

    QList<IpAddress> addresses;
    IpAddress address;
    address.setIp(QHostAddress("192.168.1.150"));
    address.setPrefixLength(24);
    addresses.append(address);

    address.setIp(QHostAddress("192.168.1.152"));
    address.setPrefixLength(24);
    addresses.append(address);

    ipv4Setting->setAddresses(addresses);

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(settings->toMap());
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        qInfo() << "Connection added successfully. Path:" << reply.value().path();
    }
}

void NMProxy::deleteConnection(const QString &name)
{
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->name() == name) {
            conn->remove();
            qInfo() << "Connection" << name << "deleted.";
            return;
        }
    }
    qWarning() << "Connection" << name << "not found.";
}

void NMProxy::modifyIpv4Setting(const QString &connectionName)
{
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->name() == connectionName) {
            ConnectionSettings::Ptr settings = conn->settings();
            Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
            ipv4Setting->setMethod(Ipv4Setting::Automatic);
            conn->update(settings->toMap());
            qInfo() << "Connection" << connectionName << "modified.";
            return;
        }
    }
    qWarning() << "Connection" << connectionName << "not found.";
}
