import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Button {
            text: "Back"
            onClicked: stackView.pop()
        }

        Button {
            text: "List Devices"
            onClicked: networkModel.listDevices()
        }

        TextField {
            id: connectionNameInput
            placeholderText: "Enter Connection Name"
        }

        TextField {
            id: interfaceNameInput
            placeholderText: "Enter Interface Name"
        }

        Button {
            text: "Activate Wired Connection"
            onClicked: networkModel.activateWiredConnection(connectionNameInput.text, interfaceNameInput.text)
        }

        Button {
            text: "Deactivate Wired Connection"
            onClicked: networkModel.deactivateWiredConnection(connectionNameInput.text)
        }

        Button {
            text: "Create Auto Wired Connection"
            onClicked: networkModel.createAutoWiredConnection(connectionNameInput.text, interfaceNameInput.text)
        }

        Button {
            text: "Create Manual Wired Connection"
            onClicked: networkModel.createWiredConnection(connectionNameInput.text, interfaceNameInput.text)
        }

        Button {
            text: "Delete Connection"
            onClicked: networkModel.deleteConnection(connectionNameInput.text)
        }

        Button {
            text: "Modify IPv4 to Auto"
            onClicked: networkModel.modifyIpv4Setting(connectionNameInput.text)
        }
    }
}
