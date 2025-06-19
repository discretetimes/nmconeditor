#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQueue>

#include <arpa/inet.h>

#include <NetworkManagerQt/Manager>
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

class NetworkModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ConnectionRoles {
        NameRole = Qt::UserRole + 1,
        UuidRole,
        TypeRole
    };

    explicit NetworkModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:
    void refresh();
    void listDevices();
    QStringList listWiredConnections();
    QString findDeviceByName(const QString &iface);
    void activateWiredConnection(const QString &connectionName, const QString &interfaceName);
    void deactivateWiredConnection(const QString &connectionName);
    void createAutoWiredConnection(const QString &name, const QString &interfaceName);
    void createWiredConnection(const QString &name, const QString &interfaceName);
    void deleteConnection(const QString &name);
    void modifyIpv4Setting(const QString &connectionName);

private:
    QList<NetworkManager::Connection::Ptr> m_connections;
};

#endif // NETWORKMODEL_H

