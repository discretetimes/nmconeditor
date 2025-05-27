===================================
networkmanager connection editor
===================================

frontend
===========
参考 nm-connection-editor

Page 1
=========
discription: first page

EtherNet(标题-连接类型)
  sec(连接1)
  auto(连接2)

add(button) (always availiable)
delete(button) (only a nmcon selected)
modify(button) (only a nmcon selected)

Page 2
========
add page
descrption: default add ethernet type connection
default: autoconnect

Name: con1 (auto gen name, can_modify)
Ethernet: devicelist:available devices
IPv4 Settings:
  Method:Auto, Mannul
  Addtion static addresses: table(iptable), add(button), delete(button)

Save Cacel


Page 3
========
modify and show details


nmcli connection add con-name test3 type 802-3-ethernet ipv4.method manual ipv4.addresses 192.168.1.100/24

QT_LOGGING_RULES="*.info=true" ./build/Desktop-Debug/nmconeditor
