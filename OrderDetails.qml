import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property Order order

    Component {
        id: detailsComponent
        Pane {
            width: parent && parent.width
            RowLayout {
                width: parent.width
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Label {
                        color: 'blue'
                        text: order.customer.defaultAddress.name
                    }
                    Text {
                        color: 'darkgray'
                        text: order.customer.defaultAddress.company
                    }
                    Text {
                        color: 'darkgray'
                        text: order.customer.defaultAddress.address1
                    }
                    Text {
                        color: 'darkgray'
                        text: order.customer.defaultAddress.address2
                    }
                    Row {
                        Text {
                            color: 'darkgray'
                            text: order.customer.defaultAddress.city
                        }
                        Text {
                            color: 'darkgray'
                            text: order.customer.defaultAddress.zip
                        }
                    }
                    Text {
                        color: 'darkgray'
                        text: order.customer.defaultAddress.country
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        color: 'lightgray'
                        border.width: 1
                        border.color: 'lightgray'
                        height: 1
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            Layout.fillWidth: true
                            font.bold: true
                            font.pointSize: 16
                            text: qsTr('Subtotal')
                        }
                        Label {
                            //anchors.right: parent.right
                            font.bold: true
                            font.pointSize: 16
                            text: order.totalPrice
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        color: 'lightgray'
                        border.width: 1
                        border.color: 'lightgray'
                        height: 1
                    }
                } // first info block
                GridLayout {
                    Layout.fillWidth: true
                    columns: 3

                    Text {
                        color: 'darkgray'
                        text: qsTr('Shipping')
                    }
                    Label {
                        color: 'blue'
                        text: order.customer.defaultAddress.name
                    }
                    Text {
                        color: 'darkgray'
                        text: order.customer.defaultAddress.address1
                    }

                    Text {
                        color: 'darkgray'
                        text: qsTr('Taxes')
                    }
                    Label {
                        color: 'blue'
                        text: order.taxLines && (order.taxLines[0].title + ' ' + order.taxLines[0].rate)
                    }
                    Text {
                        color: 'darkgray'
                        text: order.taxLines && order.taxLines[0].price
                    }
                }
            } // main row
        }
    }

    Component {
        id: lineItemsComponent
        Label {
            anchors.centerIn: parent
            text: 'Line Items'
        }
    }

    Component {
        id: responseComponent

        Flickable {
            anchors.fill: parent
            contentWidth: jsonText.width
            contentHeight: jsonText.height
            clip: true
            flickableDirection: Flickable.VerticalFlick
            TextEdit {
                id: jsonText
                readOnly: true
                text: order ? order.json : ''
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: tabs.currentIndex == 0 ? detailsComponent
                       : tabs.currentIndex == 1 ? lineItemsComponent
                       : responseComponent
    }

    footer: TabBar {
        id: tabs
        TabButton {
            text: qsTr('Details')
        }
        TabButton {
            text: qsTr('Line Items')
        }
        TabButton {
            text: qsTr('Response')
        }
    }
}
