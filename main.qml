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
                    if ( swipeView.currentItem === orderSheet )
                    {
                        if (shopify.state === Shopify.STATE_LOADING) {
                            shopify.abort()
                        } else {
                            shopify.load()
                        }
                    }
                    else
                    {
                        if (xero.state === Shopify.STATE_LOADING) {
                            xero.abort()
                        } else {
                            xero.load()
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
                        onClicked: settingsPopup.openShopify(shopify.settings)
                    }
                    MenuItem {
                        text: 'Xero'
                        onClicked: settingsPopup.openXero(xero.settings)
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
            id: invoiceSheet
            ListView {
                id: invoiceList
                anchors.fill: parent
                model: xero.invoices
                delegate: ItemDelegate {
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            text: modelData.number + ' ' + modelData.reference
                        }
                    }

                    highlighted: ListView.isCurrentItem
                    onClicked: orderList.currentIndex = index
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
    }

    Popup {
        id: settingsPopup
        property SimpleHttpConnectionSettings shopifyApiSettings
        property OAuthZeroLeggedConnectionSettings xeroApiSettings
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
                ShopifyApiSettingsPanel {
                    id: shopifyApiSettingsPanel
                    visible: settingsPopup.shopifyApiSettings != null
                    settings: settingsPopup.shopifyApiSettings
                }
                XeroApiSettingsPanel {
                    id: xeroApiSettingsPanel
                    visible: settingsPopup.xeroApiSettings != null
                    settings: settingsPopup.xeroApiSettings
                }
                RowLayout {
                    anchors.right: parent.right
                    Button {
                        text: "OK"
                        onClicked: {
                            settingsPopup.save()
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
        function save() {
            if (shopifyApiSettings) shopifyApiSettingsPanel.save()
            if (xeroApiSettings) xeroApiSettingsPanel.save()
        }
        function openShopify(settings) {
            shopifyApiSettings = settings
            xeroApiSettings = null
            open()
        }
        function openXero(settings) {
            shopifyApiSettings = null
            xeroApiSettings = settings
            open()
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
