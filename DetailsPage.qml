import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    property string connectionUuid
    property var connectionDetails: ({})

    title: connectionDetails.name || "Details"

    Component.onCompleted: {
        connectionDetails = networkModel.getConnectionDetails(connectionUuid)
    }

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            text: "Name: " + connectionDetails.name
            font.bold: true
        }
        Label {
            text: "UUID: " + connectionDetails.uuid
        }
        Label {
            text: "Type: " + connectionDetails.type
        }
        Label {
            text: "IPv4 Method: " + (connectionDetails.ipv4Method === 1 ? "Automatic" : "Manual")
        }
        Label {
            text: "Addresses:"
        }
        Column {
            anchors.left: parent.left
            anchors.leftMargin: 20
            spacing: 5
            Repeater {
                model: connectionDetails.addresses
                Label {
                    text: modelData
                }
            }
        }
    }
}
