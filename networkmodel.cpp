#include "networkmodel.h"

#include <QDebug>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/ActiveConnection>
#include <QDBusPendingReply>

using namespace NetworkManager;

NetworkModel::NetworkModel(QObject *parent)
    : QAbstractListModel(parent)
{
    refresh();
}

int NetworkModel::rowCount(const QModelIndex &parent) const
{
    // For list models, the row count is the size of the list.
    if (parent.isValid())
        return 0;

    return m_connections.count();
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const NetworkManager::Connection::Ptr &connection = m_connections.at(index.row());

    switch (role) {
    case NameRole:
        return connection->name();
    case UuidRole:
        return connection->uuid();
    case TypeRole:
        return connection->settings()->connectionType();
    }

    return QVariant();
}

bool NetworkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // Here you would implement logic to update the connection using NetworkManager
        // For example:
        // m_connections[index.row()]->settings()->setId(value.toString());
        // m_connections[index.row()]->update(m_connections[index.row()]->settings()->toMap());
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags NetworkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // Or whatever flags you need
}

QHash<int, QByteArray> NetworkModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[UuidRole] = "uuid";
    roles[TypeRole] = "type";
    return roles;
}

void NetworkModel::refresh()
{
    beginResetModel();
    qInfo() << "refresh";
    m_connections = NetworkManager::listConnections();
    qInfo() << "Wired con count:" << m_connections.count();
    for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
        // insertConnection(connection);
        qInfo() << "Con name:" << connection->name() ;
    }
    endResetModel();
}

void NetworkModel::listDevices()
{
    QList<NetworkManager::Device::Ptr> devices = NetworkManager::networkInterfaces();
    if (devices.isEmpty()) {
        qInfo() << "No network devices found.";
    }
    for (const auto &device : devices) {
        qInfo() << "Name:" << device->interfaceName() << "Type:" << device->type() << "State:" << device->state();
    }
}

QStringList NetworkModel::listWiredConnections()
{
    QStringList connectionNames;
    qInfo() << "listcons";
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->settings()->connectionType() == NetworkManager::ConnectionSettings::Wired) {
            qInfo() << "Wired con name:" << conn->name() ;
            connectionNames.append(conn->name());
        }
    }
    return connectionNames;
}

QString NetworkModel::findDeviceByName(const QString &iface)
{
    for (const auto &device : NetworkManager::networkInterfaces()) {
        if (device->interfaceName() == iface) {
            return device->uni();
        }
    }
    return QString();
}

void NetworkModel::activateWiredConnection(const QString &connectionName, const QString &interfaceName)
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

void NetworkModel::deactivateWiredConnection(const QString &connectionName)
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

void NetworkModel::createAutoWiredConnection(const QString &name, const QString &interfaceName)
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

void NetworkModel::createWiredConnection(const QString &name, const QString &interfaceName)
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

void NetworkModel::deleteConnection(const QString &name)
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

void NetworkModel::modifyIpv4Setting(const QString &connectionName)
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
