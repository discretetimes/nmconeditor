import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: editingEthernetRoot
    title: isNewConnection ? qsTr("Add Ethernet Connection") : qsTr("Editing Ethernet: ") + (connectionData ? connectionData.name : "")

    // Properties to receive data from the calling page
    property var connectionData: null // Holds the connection object being edited
    property bool isNewConnection: true

    // Signals to return to the previous page
    signal save(variant connectionData)
    signal cancel()

    // Placeholder for available Ethernet devices.
    // In a real application, this would be populated from your backend.
    ListModel {
        id: availableDevicesModel
        ListElement { text: "eth0 - Realtek PCIe GBE Family Controller" }
        ListElement { text: "eth1 - Intel(R) Wireless-AC 9560" }
        ListElement { text: "wlan0 - WiFi Adapter" }
    }

    // Model for static IP addresses
    ListModel {
        id: staticIPsModel
    }

    // Populate fields when connectionData changes (e.g., when page is loaded for modification)
    Component.onCompleted: {
        if (connectionData) {
            nameField.text = connectionData.name || ""
            const deviceIndex = findDeviceIndex(connectionData.device)
            if (deviceIndex !== -1) {
                ethernetDeviceComboBox.currentIndex = deviceIndex
            }
            ipv4MethodComboBox.currentIndex = connectionData.method === "Manual" ? 1 : 0
            if (connectionData.staticIPs) {
                staticIPsModel.clear()
                for (var i = 0; i < connectionData.staticIPs.length; ++i) {
                    staticIPsModel.append({ "ip": connectionData.staticIPs[i] })
                }
            }
        } else {
            // Defaults for a new connection
            nameField.text = "My Ethernet Connection"
            ipv4MethodComboBox.currentIndex = 0 // Auto
            staticIPsModel.clear()
        }
    }

    function findDeviceIndex(deviceName) {
        for (var i = 0; i < availableDevicesModel.count; i++) {
            // This assumes deviceName matches the start of the text property. Adjust as needed.
            if (availableDevicesModel.get(i).text.startsWith(deviceName)) {
                return i;
            }
        }
        return -1; // Not found
    }


    ScrollView {
        anchors.fill: parent
        clip: true

        GridLayout {
            width: parent.width
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Label { text: qsTr("Name:") }
            TextField {
                id: nameField
                Layout.fillWidth: true
                placeholderText: qsTr("e.g., Home Ethernet")
            }

            Label { text: qsTr("Ethernet:") }
            ComboBox {
                id: ethernetDeviceComboBox
                Layout.fillWidth: true
                model: availableDevicesModel
                textRole: "text" // Assuming your model items have a "text" property
            }

            Label {
                text: qsTr("IPv4 Settings:")
                font.bold: true
                Layout.columnSpan: 2
                Layout.topMargin: 10
            }

            Label { text: qsTr("Method:") }
            ComboBox {
                id: ipv4MethodComboBox
                Layout.fillWidth: true
                model: [qsTr("Auto (DHCP)"), qsTr("Manual")]
            }

            Label {
                text: qsTr("Additional static addresses:")
                Layout.columnSpan: 2
                visible: ipv4MethodComboBox.currentIndex === 1 // Only show for Manual
                Layout.topMargin: 5
            }

            ListView {
                id: staticIpListView
                Layout.fillWidth: true
                Layout.minimumHeight: 100
                Layout.columnSpan: 2
                clip: true
                model: staticIPsModel
                visible: ipv4MethodComboBox.currentIndex === 1 // Only show for Manual
                currentIndex: -1

                delegate: ItemDelegate {
                    width: parent.width
                    text: model.ip
                    highlighted: ListView.isCurrentItem
                    onClicked: staticIpListView.currentIndex = index

                    RowLayout {
                        anchors.fill: parent
                        Label {
                            text: model.ip
                            Layout.fillWidth: true
                        }
                    }
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }

            RowLayout {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: ipv4MethodComboBox.currentIndex === 1 // Only show for Manual

                TextField {
                    id: newIpAddressField
                    Layout.fillWidth: true
                    placeholderText: qsTr("e.g., 192.168.1.10/24")
                }

                Button {
                    text: qsTr("Add")
                    enabled: newIpAddressField.text.trim() !== ""
                    onClicked: {
                        if (newIpAddressField.text.trim() !== "") {
                            staticIPsModel.append({ "ip": newIpAddressField.text.trim() })
                            newIpAddressField.text = ""
                        }
                    }
                }
                Button {
                    text: qsTr("Delete")
                    enabled: staticIpListView.currentIndex !== -1 && staticIPsModel.count > 0
                    onClicked: {
                        if (staticIpListView.currentIndex !== -1) {
                            staticIPsModel.remove(staticIpListView.currentIndex)
                            staticIpListView.currentIndex = -1 // Reset selection
                        }
                    }
                }
            }


            // Spacer
            Item { Layout.fillHeight: true; Layout.columnSpan: 2 }


            RowLayout {
                Layout.alignment: Qt.AlignBottom | Qt.AlignRight
                Layout.columnSpan: 2
                Layout.topMargin: 20

                Button {
                    text: qsTr("Save")
                    highlighted: true // Or use Control.primaryFocus
                    onClicked: {
                        var currentStaticIPs = [];
                        for (var i = 0; i < staticIPsModel.count; ++i) {
                            currentStaticIPs.push(staticIPsModel.get(i).ip);
                        }
                        const dataToSave = {
                            name: nameField.text,
                            device: ethernetDeviceComboBox.currentText, // Or a more specific ID if your model has it
                            method: ipv4MethodComboBox.currentIndex === 0 ? "Auto" : "Manual",
                            staticIPs: currentStaticIPs
                        };
                        editingEthernetRoot.save(dataToSave)
                    }
                }
                Button {
                    text: qsTr("Cancel")
                    onClicked: editingEthernetRoot.cancel()
                }
            }
        }
    }
}
