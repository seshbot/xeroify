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
                text: shopify.state == Shopify.STATE_DISCONNECTED ? "Connect" : shopify.state == Shopify.STATE_CONNECTING ? "Connecting..." : "Disconnect"
                checkable: true
                checked: shopify.state != Shopify.STATE_DISCONNECTED
                onClicked: if ( shopify.state == Shopify.STATE_DISCONNECTED ) { shopify.connect(); } else { shopify.disconnect(); }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                text: "Settings"
                onClicked: settingsMenu.open()
                id: settingsMenuButton
                Menu {
                    id: settingsMenu
                    y: settingsMenuButton.height
                    MenuItem {
                        text: "Shopify"
                        onClicked:  {
                            settingsPopup.settings = shopify.settings
                            settingsPopup.open()
                        }
                    }
                    MenuItem {
                        text: "Xero"
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

        Page1 {
        }

        Page {
            Label {
                text: qsTr("Second page")
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
            text: qsTr("First")
        }
        TabButton {
            text: qsTr("Second")
        }
    }
}
