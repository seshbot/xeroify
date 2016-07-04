import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Xeroify")

    property Shopify shopify: Shopify { }
    property Xero xero: Xero { }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: shopify.state === Shopify.STATE_LOADING ? 'Abort' : 'Load'
                onClicked: {
                    if (shopify.state === Shopify.STATE_LOADING) {
                        shopify.abort()
                    } else {
                        shopify.load()
                    }
                }
            }
            ToolButton {
                id: statusMenuButton
                text: 'Status'
                onClicked: statusMenu.open()
                Menu {
                    id: statusMenu
                    y: statusMenuButton.height
                    MenuItem {
                        CheckBox {
                            text: 'all'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'open'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'cancelled'
                        }
                    }
                }
            }
            ToolButton {
                id: financialMenuButton
                text: 'Financial'
                onClicked: financialMenu.open()
                Menu {
                    id: financialMenu
                    y: financialMenuButton.height
                    MenuItem {
                        CheckBox {
                            text: 'all'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'authorized'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'pending'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'paid'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'part paid'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'unpaid'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'refunded'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'part refunded'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'voided'
                        }
                    }
                }
            }
            ToolButton {
                id: fulfillmentMenuButton
                text: 'Fulfillment'
                onClicked: fulfillmentMenu.open()
                Menu {
                    id: fulfillmentMenu
                    y: fulfillmentMenuButton.height
                    MenuItem {
                        CheckBox {
                            text: 'all'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'shipped'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'partial'
                        }
                    }
                    MenuItem {
                        CheckBox {
                            text: 'unshipped'
                        }
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: settingsMenuButton
                text: 'Settings'
                onClicked: settingsMenu.open()
                Menu {
                    id: settingsMenu
                    y: settingsMenuButton.height
                    MenuItem {
                        text: 'Shopify'
                        onClicked:  {
                            settingsPopup.settings = shopify.settings
                            settingsPopup.open()
                        }
                    }
                    MenuItem {
                        text: 'Xero'
                        onClicked: {
                            settingsPopup.settings = xero.settings
                            settingsPopup.open()
                        }
                    }
                }
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            id: orderSheet
            ListView {
                id: orderList
                anchors.fill: parent
                model: shopify.orders
                delegate: ItemDelegate {
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            text: modelData.name + ' (' + modelData.orderStatusString + ',' + modelData.financialStatusString + ',' + modelData.fulfillmentStatusString + ')'
                        }
                    }

                    highlighted: ListView.isCurrentItem
                    onClicked: orderList.currentIndex = index
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }

        Page {
            id: page1
            Label {
                text: qsTr('Second page')
                anchors.centerIn: parent
            }
        }
    }

    Popup {
        id: settingsPopup
        property ConnectionSettings settings
        x: (parent.width - width) / 2
        //y: parent.header / 6
        modal: true
        focus: true
        contentItem: RowLayout {
            ColumnLayout {
                spacing: 10
                Label {
                    text: "Settings "
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    padding: 10
                }
                ConnectionSettingsPanel {
                    id: settingsPanel
                    settings: settingsPopup.settings
                }
                RowLayout {
                    anchors.right: parent.right
                    Button {
                        text: "OK"
                        onClicked: {
                            settingsPanel.save()
                            settingsPopup.close()
                        }
                    }
                    Button {
                        text: "Cancel"
                        onClicked: {
                            settingsPopup.close()
                        }
                    }
                }
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Shopify")
        }
        TabButton {
            text: qsTr("Xero")
        }
    }
}
