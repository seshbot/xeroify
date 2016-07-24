import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr('Xeroify')

    property Shopify shopify: Shopify { }
    property Xero xero: Xero { }
    property MakeLeaps makeLeaps: MakeLeaps { }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 5 * 3
        height: window.height
        ListView {
            id: drawerList
            anchors.fill: parent
            currentIndex: -1
            highlightFollowsCurrentItem: true

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: swipeView.currentIndex === model.page
                onClicked: {
                    swipeView.currentIndex = model.page
                    windowTitle.text = model.title
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement { title: qsTr('Shopify'); page: 1 }
                ListElement { title: qsTr('Xero'); page: 2 }
                ListElement { title: qsTr('MakeLeaps'); page: 3 }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr('Menu')
                onClicked: {
                    drawer.open()
                }
            }

            ToolButton {
                text: shopify.state === Shopify.STATE_LOADING ? qsTr('Abort') : qsTr('Load')
                onClicked: {
                    if ( swipeView.currentItem === shopifyPage )
                    {
                        if (shopify.state === Shopify.STATE_LOADING) {
                            shopify.abort()
                        } else {
                            shopify.load()
                        }
                    }
                    else if ( swipeView.currentItem === xeroPage )
                    {
                        if (xero.state === Xero.STATE_LOADING) {
                            xero.abort()
                        } else {
                            xero.load()
                        }
                    }
                    else
                    {
                        if (makeLeaps.state === MakeLeaps.STATE_LOADING) {
                            makeLeaps.abort()
                        } else {
                            makeLeaps.load()
                        }
                    }
                }
            }
            Label {
                id: windowTitle
                Layout.fillWidth: true
                horizontalAlignment: Label.AlignHCenter
                font.bold: true
                text: qsTr('BusyBot Command Center')
            }
            ToolButton {
                id: settingsMenuButton
                text: qsTr('Settings')
                onClicked: settingsMenu.open()
                Menu {
                    id: settingsMenu
                    y: settingsMenuButton.height
                    MenuItem {
                        text: qsTr('Shopify')
                        onClicked: settingsPopup.openShopify(shopify.settings)
                    }
                    MenuItem {
                        text: qsTr('MakeLeaps')
                        onClicked: {
                            console.log('makeLeaps: ', makeLeaps)
                            console.log('makeLeaps settings: ', makeLeaps.settings)
                            settingsPopup.openMakeLeaps(makeLeaps.settings)
                        }
                    }
                }
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        Page {
            id: homePage
            Pane {
                anchors.centerIn: parent
                Text {
                    text: qsTr('Welcome')
                }
            }
            footer: ToolBar {
                ToolButton {
                    anchors.right: parent.right
                    text: qsTr('Load ') + makeLeaps.state
                    onClicked: {
                        if (makeLeaps.state === MakeLeaps.STATE_LOADING) {
                            makeLeaps.abort()
                        } else {
                            makeLeaps.load()
                        }
                    }
                }
            }
        }
        Page {
            id: shopifyPage
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
            id: xeroPage
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
        MakeLeapsBrowser {
            Component.onCompleted: {
                makeLeaps.load()
            }
            endpoint: makeLeaps.root
        }
    }

    Popup {
        id: settingsPopup
        property SimpleHttpConnectionSettings shopifyApiSettings
        property OAuthZeroLeggedConnectionSettings xeroApiSettings
        property OAuth2Settings makeLeapsApiSettings
        x: (parent.width - width) / 2
        //y: parent.header / 6
        modal: true
        focus: true
        contentItem: RowLayout {
            ColumnLayout {
                spacing: 10
                Label {
                    text: qsTr('Settings')
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
                    visible: false
                    settings: settingsPopup.xeroApiSettings
                }
                MakeLeapsApiSettingsPanel {
                    id: makeLeapsApiSettingsPanel
                    visible: settingsPopup.makeLeapsApiSettings != null
                    settings: settingsPopup.makeLeapsApiSettings
                }
                RowLayout {
                    anchors.right: parent.right
                    Button {
                        text: qsTr('OK')
                        onClicked: {
                            settingsPopup.save()
                            settingsPopup.close()
                        }
                    }
                    Button {
                        text: qsTr('Cancel')
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
            if (makeLeapsApiSettings) {
                makeLeapsApiSettingsPanel.save()
                makeLeaps.reloadAccessToken()
            }
        }
        function openShopify(settings) {
            shopifyApiSettings = settings
            xeroApiSettings = null
            makeLeapsApiSettings = null
            open()
        }
        function openXero(settings) {
            shopifyApiSettings = null
            xeroApiSettings = settings
            makeLeapsApiSettings = null
            open()
        }
        function openMakeLeaps(settings) {
            shopifyApiSettings = null
            xeroApiSettings = null
            makeLeapsApiSettings = settings
            open()
        }
    }
}
