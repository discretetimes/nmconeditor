import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property string connectionUuid
    property var connectionDetails: ({})

    title: "Edit: " + (connectionDetails.name || "Connection")

    Component.onCompleted: {
        connectionDetails = networkModel.getConnectionDetails(connectionUuid)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Label { text: "Connection Name:" }
        TextField {
            id: nameField
            text: connectionDetails.name
            readOnly: true // Name is not easily editable via DBus
        }

        Label { text: "IPv4 Method:" }
        ComboBox {
            id: methodComboBox
            currentIndex: connectionDetails.ipv4Method === 1 ? 0 : 1
            model: ["Automatic (DHCP)", "Manual"]
        }

        Label { text: "Addresses (IP/Prefix):" }
        ListView {
            id: addressListView
            Layout.fillWidth: true
            height: 100
            model: connectionDetails.addresses
            delegate: TextField {
                width: parent.width
                text: modelData
                // In a real app, you'd have add/remove buttons for addresses
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        onAccepted: {
            var newSettings = {
                "ipv4Method": methodComboBox.currentIndex === 0 ? 1 : 2, // 1 for Auto, 2 for Manual
                "addresses": []
            }
            // In a real app, you would get the text from the ListView delegates
            // For simplicity, we are not implementing this here.
            networkModel.updateConnection(connectionUuid, newSettings);
            stackView.pop();
        }
        onRejected: {
            stackView.pop()
        }
    }
}
