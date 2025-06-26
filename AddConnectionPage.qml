import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    title: "Add New Ethernet Connection"

    ListModel {
        id: addressModel
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        Label { text: "Connection Name:" }
        TextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "e.g., My Office Connection"
        }

        Label { text: "IPv4 Method:" }
        ComboBox {
            id: methodComboBox
            Layout.fillWidth: true
            model: ["Automatic (DHCP)", "Manual"]
        }

        ListView {
            id: addressListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: addressModel
            currentIndex: -1
            enabled: methodComboBox.currentIndex === 1

            delegate: ItemDelegate {
                width: parent.width
                highlighted: ListView.isCurrentItem

                TextField {
                    id: addressField
                    anchors.fill: parent
                    text: model.address
                    placeholderText: parent.enabled ? "e.g., 192.168.1.100/24" : "Not applicable"
                    background: null
                    readOnly: !parent.enabled
                    onEditingFinished: {
                        model.address = text
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    preventStealing: true
                    onClicked: {
                        addressListView.currentIndex = index
                        addressField.forceActiveFocus()
                    }
                }
            }
        }

        RowLayout {
            enabled: methodComboBox.currentIndex === 1
            Button {
                text: qsTr("Add Address")
                onClicked: {
                    addressModel.append({ "address": "" })
                }
            }
            Button {
                text: qsTr("Remove Selected")
                enabled: addressListView.currentIndex !== -1
                onClicked: {
                    if (addressListView.currentIndex !== -1) {
                        addressModel.remove(addressListView.currentIndex)
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        onAccepted: {
            // A name is required to create a connection
            if (nameField.text.trim() === "") {
                // You could show a warning dialog here
                return;
            }

            var newAddresses = []
            if(methodComboBox.currentIndex === 1) {
                for (var i = 0; i < addressModel.count; i++) {
                    var item = addressModel.get(i)
                    if (item.address !== "") {
                        newAddresses.push(item.address)
                    }
                }
            }
            var newMethod;
            if (methodComboBox.currentIndex === 0) {
                newMethod = 0;
            } else {
                newMethod = 2;
            }
            var newSettings = {
                "ipv4Method": newMethod,
                "addresses": newAddresses
            }

            // Call the new C++ function to create the connection
            networkModel.createConnection(nameField.text, newSettings)
            stackView.pop()
        }
        onRejected: {
            stackView.pop()
        }
    }
}
