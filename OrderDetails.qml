import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property Order order

    Component {
        id: detailsComponent

        Flickable {
            anchors.fill: parent
            contentWidth: width
            contentHeight: detailsPanel.height
            clip: true
            flickableDirection: Flickable.VerticalFlick

            Pane {
                id: detailsPanel
                anchors.fill: parent

                ColumnLayout {
                    width: parent.width
                    spacing: 10
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
                                font.pointSize: 12
                                text: qsTr('Subtotal')
                            }
                            Label {
                                font.pointSize: 12
                                text: order.subtotalPrice
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
                        rowSpacing: 10
                        columnSpacing: 15

                        Text {
                            color: 'darkgray'
                            text: qsTr('Shipping')
                        }
                        Label {
                            Layout.fillWidth: true
                            color: 'blue'
                            text: order.shippingLines && order.shippingLines[0].title
                        }
                        Text {
                            Layout.alignment: Qt.AlignRight
                            color: 'darkgray'
                            text: order.shippingLines && order.shippingLines[0].price
                        }

                        Text {
                            color: 'darkgray'
                            text: qsTr('Taxes')
                        }
                        Label {
                            Layout.fillWidth: true
                            color: 'blue'
                            text: order.taxLines && (order.taxLines[0].title + ' ' + order.taxLines[0].rate)
                        }
                        Text {
                            Layout.alignment: Qt.AlignRight
                            color: 'darkgray'
                            text: order.taxLines && order.taxLines[0].price
                        }
                    } // shipping/taxes block
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 10

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
                                font.pointSize: 12
                                text: qsTr('Total')
                            }
                            Label {
                                font.pointSize: 12
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
                    } // subtotal block
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 3
                        rowSpacing: 10
                        columnSpacing: 15

                        Text {
                            color: 'darkgray'
                            text: qsTr('Notes')
                        }
                        Label {
                            Layout.fillWidth: true
                            Layout.columnSpan: 2
                            color: 'blue'
                            text: order.note
                        }
                    } // payment/notes
                } // main column
            } // main pane
        } // flickable
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
                focus: true
                readOnly: true
                font.family: 'Courier'
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
