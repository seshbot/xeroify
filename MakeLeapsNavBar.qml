import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root

    property MakeLeapsPartner partner

    property var history: []
    property var future: []
    property bool backEnabled: history.length > 0
    property bool forwardEnabled: future.length > 0

    signal resourceSelected(string type, ApiObject resource)

    function pushHistory(h, clearFuture) {
        clearFuture = typeof clearFuture !== 'undefined' ? clearFuture : true
        history.push(h)
        if (clearFuture) {
            future = []
        }
        if (h.navBarStateChanged) {
            var n = h.navBarState;
            navBar.push(n.component, n.properties)
        }
        if (h.resourceStateChanged) {
            var r = h.resourceState;
            resourceSelected(r.type, r.object)
        }
    }

    function popHistory() {
        if (history.length == 0) {
            console.log('cannot pop history (history empty)')
            return
        }

        var h = history.pop()
        if (h.navBarStateChanged) {
            navBar.pop()
        }
        if (h.resourceStateChanged) {
            var previousResourceState

            for (var idx = history.length - 1; idx >= 0; idx--) {
                var state = history[idx]
                if (state.resourceStateChanged) {
                    previousResourceState = state.resourceState
                    break
                }
            }

            if ( previousResourceState ) {
                resourceSelected(previousResourceState.type, previousResourceState.object)
            }
        }

        return h
    }

    function pushNavBar(component, properties) {
        pushHistory({ navBarStateChanged: true, navBarState: { component: component, properties: properties } }, true)
        updateNavButtons()
    }
    function pushResourceDetails(type, resourceObject) {
        pushHistory({ resourceStateChanged: true, resourceState: { type: type, object: resourceObject } }, true)
        updateNavButtons()
    }

    function forward() {
        if (future.length <= 0) return

        var h = future.pop()
        pushHistory(h, false)

        updateNavButtons()
    }
    function back() {
        if (history.length <= 0) return

        var h = popHistory()
        future.push(h)

        updateNavButtons()
    }
    function updateNavButtons() {
        console.log('history is now ', history.length)
        backEnabled = history.length > 0
        forwardEnabled = future.length > 0
    }

    header: Pane {
        width: parent.width
        RowLayout {
            width: parent.width
            Button {
                text: "<"
                enabled: root.backEnabled
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                onClicked: {
                    back()
                }
            }
            Button {
                text: ">"
                enabled: root.forwardEnabled
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                onClicked: {
                    forward()
                }
            }
            Label {
                id: headerLabel
                Layout.fillWidth: true
                padding: 6
                horizontalAlignment: Text.AlignHCenter
                color: 'gray'
                text: navBar.currentItem.type || ''
            }
            Button {
                text: "+"
                enabled: false
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                onClicked: {
                }
            }
        }
    }

    StackView {
        id: navBar

        anchors.fill: parent

        Component {
            id: endpointNavComponent
            Pane {
                id: endpointLoader

                property string type
                property MakeLeapsEndpoint endpoint

                onEndpointChanged: {
                    if ( endpoint && endpoint.state === MakeLeapsEndpoint.STATE_IDLE )
                        endpoint.getResource()
                }

                Loader {
                    anchors.fill: parent
                    Component {
                        id: endpointLoadingComponent
                        Label {
                            anchors.centerIn: parent
                            text: 'loading...'
                        }
                    }
                    Component {
                        id: endpointChildrenComponent
                        ListView {
                            id: endpointChildren
                            model: endpoint.rootProperty.asArray
                            delegate: ItemDelegate {
                                width: endpointChildren.width
                                text: modelData.name
                                onClicked: {
                                    pushResourceDetails( type, modelData.asObject )
                                }
                            }

                            ScrollIndicator.vertical: ScrollIndicator { }
                        }
                    }
                    sourceComponent: {
                        if (!endpoint || endpoint.state !== MakeLeapsEndpoint.STATE_LOADED)
                            return endpointLoadingComponent
                        return endpointChildrenComponent
                    }
                }
            }
        }

        initialItem: Loader {
            Component {
                id: loadedPage
                ListView {
                    id: partnerSections
                    anchors.fill: parent
                    model: ListModel {
                        ListElement { name: 'Contacts'; path: 'contacts' }
                        ListElement { name: 'Clients'; path: 'clients' }
                        ListElement { name: 'Documents'; path: 'documents' }
                    }
                    delegate: ItemDelegate {
                        width: partnerSections.width
                        text: model.name
                        onClicked: {
                            console.log('click:', model.path, partner)
                            if ( model.path === 'contacts' ) pushNavBar( endpointNavComponent, { type: 'contacts', endpoint: partner.contacts } )
                            else if ( model.path === 'clients' ) pushNavBar( endpointNavComponent, { type: 'clients', endpoint: partner.clients } )
                            else if ( model.path === 'documents' ) pushNavBar( endpointNavComponent, { type: 'documents', endpoint: partner.documents } )
                        }
                    }

                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            }
            Component {
                id: loadingPage
                Label {
                    anchors.centerIn: parent
                    text: 'loading'
                }
            }
            Component {
                id: nullPage
                Label {
                    text: 'null'
                }
            }
            Component {
                id: errorPage
                Label {
                    text: 'error'
                }
            }
            sourceComponent: {
                if ( !partner ) return nullPage
                if ( partner.state === MakeLeapsPartner.STATE_LOADING ) return loadingPage
                if ( partner.state === MakeLeapsPartner.STATE_LOADED ) return loadedPage
                return errorPage
            }
        }
    }

}
