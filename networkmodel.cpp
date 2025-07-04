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

    // qInfo() << "Connection addres:" << addresses;

    ipv4Setting->setAddresses(addresses);

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(settings->toMap());
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        qInfo() << "Connection added successfully. Path:" << reply.value().path();
    }
}

// void NetworkModel::deleteConnection(const QString &name)
// {
//     for (const auto &conn : NetworkManager::listConnections()) {
//         if (conn->name() == name) {
//             conn->remove();
//             qInfo() << "Connection" << name << "deleted.";
//             return;
//         }
//     }
//     qWarning() << "Connection" << name << "not found.";
// }

void NetworkModel::deleteConnection(const QString &uuid)
{
    // We re-fetch the list here to ensure we have the most up-to-date
    // DBus object paths before attempting to delete.
    for (const auto &conn : NetworkManager::listConnections()) {
        if (conn->uuid() == uuid) {
            conn->remove(); // This is the call that tells NetworkManager to delete the connection
            qInfo() << "Connection with UUID" << uuid << "deleted.";
            return;
        }
    }
    qWarning() << "Connection with UUID" << uuid << "not found for deletion.";
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

void NetworkModel::updateIpv4Method(const QString &uuid, int method)
{
    for (const auto &conn : NetworkManager::listConnections()) {
        qInfo() << "Connection" << conn->uuid();
        if (conn->uuid() == uuid) {
            ConnectionSettings::Ptr settings = conn->settings();
            Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
            if(method == 0)
                ipv4Setting->setMethod(Ipv4Setting::Automatic);
            if(method == 2)
                ipv4Setting->setMethod(Ipv4Setting::Manual);
            conn->update(settings->toMap());
            qInfo() << "Connection" << conn->name() << "modified.";
            return;
        }
    }
    // qWarning() << "Connection" << conn << "not found.";
}

// void NetworkModel::updateIpv4Addr(const QString &uuid, QStringList &addresses)
// {
//     for (const auto &conn : NetworkManager::listConnections()) {
//             qInfo() << "Connection" << conn->uuid();
//         if (conn->uuid() == uuid) {
//             ConnectionSettings::Ptr settings = conn->settings();
//             Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
//             // if(method == 1)
//             //     ipv4Setting->setMethod(Ipv4Setting::Automatic);
//             // if(method == 2)
//             //     ipv4Setting->setMethod(Ipv4Setting::Manual);

//             // ipv4Setting->setAddresses(addresses);
//             conn->update(settings->toMap());
//             qInfo() << "Connection" << conn->name() << "modified.";
//             return;
//         }
//     }
//     // qWarning() << "Connection" << conn << "not found.";
// }

QVariantMap NetworkModel::getConnectionDetails(const QString &uuid)
{
    QVariantMap details;
    for (const auto &conn : m_connections) {
        if (conn->uuid() == uuid) {
            details["name"] = conn->name();
            details["uuid"] = conn->uuid();
            details["type"] = conn->settings()->connectionType();

            // Add more details as needed
            Ipv4Setting::Ptr ipv4Setting = conn->settings()->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
            if (ipv4Setting) {
                details["ipv4Method"] = ipv4Setting->method();
                qInfo() << "Method" << ipv4Setting->method();
                QList<IpAddress> addresses = ipv4Setting->addresses();
                QStringList addrStrings;
                for(const auto& addr : addresses) {
                    addrStrings.append(addr.ip().toString() + "/" + QString::number(addr.prefixLength()));
                }
                details["addresses"] = addrStrings;
            }
            return details;
        }
    }
    return details; // Return empty map if not found
}

void NetworkModel::updateConnection(const QString &uuid, const QVariantMap &newSettings)
{
    for (const auto &conn : m_connections) {
        if (conn->uuid() == uuid) {
            ConnectionSettings::Ptr settings = conn->settings();
            Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();

            // if (newSettings.contains("ipv4Method")) {
            //     ipv4Setting->setMethod(static_cast<Ipv4Setting::Manual>(newSettings["ipv4Method"].toInt()));
            // }

            if (newSettings.contains("addresses")) {
                QList<IpAddress> addresses;
                for(const QVariant &addrVariant : newSettings["addresses"].toList()) {
                    QStringList parts = addrVariant.toString().split('/');
                    if(parts.length() == 2) {
                        IpAddress address;
                        address.setIp(QHostAddress(parts[0]));
                        address.setPrefixLength(parts[1].toUInt());
                        addresses.append(address);
                    }
                }
                ipv4Setting->setAddresses(addresses);
            }

            conn->update(settings->toMap());
            qInfo() << "Connection" << conn->name() << "updated.";
            return;
        }
    }
    qWarning() << "Connection with UUID" << uuid << "not found.";
}

void NetworkModel::createConnection(const QString &name, const QVariantMap &settings)
{
    ConnectionSettings::Ptr newSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wired));
    newSettings->setId(name);
    newSettings->setUuid(ConnectionSettings::createNewUuid());

    Ipv4Setting::Ptr ipv4Setting = newSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setInitialized(true);

    // if (settings.contains("ipv4Method")) {
    //     ipv4Setting->setMethod(static_cast<Ipv4Setting::Method>(settings["ipv4Method"].toInt()));
    // }

    int method = settings["ipv4Method"].toInt();

    if(method == 0){
        qInfo() << "method: auto " << method;
        ipv4Setting->setMethod(Ipv4Setting::Automatic);
    } else {
    ipv4Setting->setMethod(Ipv4Setting::Manual);

    if (settings.contains("addresses")) {
        QList<IpAddress> addresses;
        for(const QVariant &addrVariant : settings["addresses"].toList()) {
            QStringList parts = addrVariant.toString().split('/');
            if(parts.length() == 2) {
                IpAddress address;
                address.setIp(QHostAddress(parts[0]));
                address.setPrefixLength(parts[1].toUInt());
                addresses.append(address);
            }
        }
        ipv4Setting->setAddresses(addresses);
    }
    }

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(newSettings->toMap());
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Failed to add connection:" << reply.error().name() << reply.error().message();
    } else {
        qInfo() << "Connection added successfully. Path:" << reply.value().path();
        refresh(); // Refresh the model to show the new connection in the list
        emit connectionsChanged(); // Emit the signal to notify QML
    }
}

// NMVariantMapMap updateSetting(const NMVariantMapMap &settings , const QVariantMap &newSettings)
// {
//     NetworkManager::ConnectionSettings::Ptr connectionSettings =
//     NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(m_connection->connectionType()));
//     connectionSettings->setUuid(m_connection->uuid());
//     return connectionSettings->toMap();
// }

QVariantMap NetworkModel::get(int row)
{
    if (row < 0 || row >= m_connections.count())
        return QVariantMap();

    QVariantMap res;
    const auto &connection = m_connections.at(row);
    res["name"] = connection->name();
    res["uuid"] = connection->uuid();
    return res;
}

void NetworkModel::remove(int index)
{
    if(index < 0 || index >= m_connections.count()) {
        return;
    }

    NetworkManager::Connection::Ptr connectionToDelete = m_connections.at(index);
    QString uuid = connectionToDelete->uuid();

    emit beginRemoveRows(QModelIndex(), index, index);
    m_connections.removeAt(index);
    emit endRemoveRows();

    deleteConnection(uuid);
    // emit countChanged(m_data.count());
}
