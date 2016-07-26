import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property MakeLeapsEndpoint endpoint
    property bool invalid : !endpoint || endpoint.state == MakeLeapsEndpoint.STATE_INVALID
    property bool error : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_ERROR
    property bool loading : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADING
    property bool loaded : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADED
    property bool singleEntity : endpoint && endpoint.resource

    property int navbarWidth : root.width / 4

    property bool backEnabled: false
    property bool forwardEnabled: false

    signal endpointSelected(MakeLeapsEndpoint e)
    signal backClicked()
    signal forwardClicked()

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
                    backClicked()
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
                    forwardClicked()
                }
            }
            Label {
                id: headerLabel
                Layout.fillWidth: true
                padding: 6
                horizontalAlignment: Text.AlignHCenter
                color: 'gray'
                text: invalid ? '' : error ? qsTr('Error: ') + endpoint.lastErrorMessage: loading ? qsTr('loading') : endpoint.url
            }
        }
    }
    Component {
        id: loadingPage
        Label {
            text: 'loading'
        }
    }
    Component {
        id: singleResource
        RowLayout {
            id: singleResourcePage
            property var selectedResourceProperty
            anchors.fill: parent
            //
            // endpoint nav bar
            //
            ListView {
                id: childList
                Layout.minimumWidth: navbarWidth
                Layout.fillHeight: true
                currentIndex: -1
                highlightFollowsCurrentItem: true
                model: endpoint.resource.properties
                delegate: ItemDelegate {
                    text: modelData.name
                    width: parent.width
                    highlighted: singleResourcePage.selectedResourceProperty === modelData
                    onClicked: {
                        if (modelData.type === MakeLeapsResourceProperty.TYPE_ENDPOINT)
                        {
                            root.endpointSelected(modelData.endpoint)
                        }
                        else
                        {
                            singleResourcePage.selectedResourceProperty = modelData
                        }
                    }
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
            Page {
                Layout.fillWidth: true
                Layout.fillHeight: true
                SwipeView {
                    id: makeLeapsView
                    currentIndex: makeLeapsTabs.currentIndex
                    anchors.fill: parent
                    Loader {
                        id: resourceDetails
                        Component {
                            id: rawData
                            Flickable {
                                TextArea.flickable: TextArea {
                                    text: endpoint.resource.jsonString
                                    wrapMode: TextArea.Wrap
                                }
                                ScrollBar.vertical: ScrollBar { }
                            }
                        }
                        Component {
                            id: resourceInfo
                            Pane {
                                anchors.fill: resourceDetails
                                Component {
                                    id: emptyInfo
                                    Text {
                                        text: 'No Endpoint'
                                    }
                                }
                                Component {
                                    id: todoPropertyInfo
                                    Text {
                                        text: 'todo'
                                    }
                                }
                                Component {
                                    id: noPropertyInfo
                                    Text {
                                        text: ''
                                    }
                                }
                                Component {
                                    id: endpointPropertyInfo
                                    Pane {
                                        property var endpoint: selectedResourceProperty.endpoint
                                        anchors.centerIn: parent
                                        Label {
                                            anchors.centerIn: parent
                                            text: endpoint ? endpoint.url : '-'
                                        }
                                    }
                                }
                                Component {
                                    id: resourceArrayPropertyInfo
                                    ListView {
                                        anchors.fill: parent
                                        model: selectedResourceProperty.resources
                                        delegate: ItemDelegate {
                                            text: modelData.name
                                            onClicked: {
                                                endpointSelected(modelData.properties[1].endpoint)
                                            }
                                        }
                                    }
                                }
                                Component {
                                    id: valueArrayPropertyInfo
                                    ListView {
                                        model: selectedResourceProperty.arrayValue
                                        delegate: ItemDelegate {
                                            text: modelData.json
                                        }
                                    }
                                }
                                Component {
                                    id: valuePropertyInfo
                                    Text {
                                        text: selectedResourceProperty.type === JsonValue.TYPE_STRING
                                              ? 'string value: ' + selectedResourceProperty.name
                                              : 'not a string: ' + selectedResourceProperty.name
                                    }
                                }
                                Loader {
                                    sourceComponent: {
                                        if (!singleResourcePage || !singleResourcePage.selectedResourceProperty)
                                            return noPropertyInfo;

                                        switch (singleResourcePage.selectedResourceProperty.type)
                                        {
                                        case MakeLeapsResourceProperty.TYPE_ENDPOINT: return endpointPropertyInfo;
                                        case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return valueArrayPropertyInfo;
                                        case MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY: return resourceArrayPropertyInfo;
                                        default: return todoPropertyInfo;
                                        }
                                    }
                                }
                            }
                        }
                        sourceComponent: makeLeapsTabs.currentIndex === 0 ? resourceInfo : rawData
                    }
                }
                footer: TabBar {
                    id: makeLeapsTabs
                    currentIndex: makeLeapsView.currentIndex

                    TabButton {
                        text: 'Selected'
                    }
                    TabButton {
                        text: 'Raw Data'
                    }
                }
            }
        }
    }
    Component {
        id: resourceArray
        RowLayout {
            id: resourceArrayPage
            property var selectedResource
            ListView {
                Layout.minimumWidth: root.width / 4
                Layout.fillHeight: true
                model: endpoint.resources
                delegate: ItemDelegate {
                    text: modelData.name
                    width: parent.width
                    highlighted: resourceArrayPage.selectedResource === modelData
                    onClicked: {
                        resourceArrayPage.selectedResource = modelData
                    }
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Component {
                    id: resourceView
                    Page {
                        anchors.fill: parent
                        ListView {
                            anchors.fill: parent
                            model: resourceArrayPage.selectedResource.properties
                            delegate: ItemDelegate {
                                width: parent.width
                                Row {
                                    width: parent.width
                                    anchors.verticalCenter: parent.verticalCenter
                                    Label {
                                        color: Material.color(Material.Grey)
                                        width: 100
                                        text: modelData.name
                                    }
                                    Label {
                                        text: {
                                            switch (modelData.type) {
                                            case MakeLeapsResourceProperty.TYPE_VALUE: return modelData.value;
                                            case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return modelData.properties.length + ' values';
                                            default: return modelData.typeString;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                Component {
                    id: loadingResourceView
                    Page {
                        anchors.centerIn: parent
                        Label {
                            text: ''
                        }
                    }
                }
                sourceComponent: resourceArrayPage.selectedResource ? resourceView : loadingResourceView
            }
        }
    }
    Component {
        id: errorPage
        Page {
            anchors.fill: parent
            Button {
                anchors.centerIn: parent
                text: 'Reload'
                onClicked: {
                    root.endpoint.load()
                }
            }
        }
    }

    Loader {
        id: resourceLoader
        anchors.fill: parent
        sourceComponent: {
            if (root.error) return errorPage;
            if (!root.loaded) return loadingPage;
            if (root.singleEntity) return singleResource;
            return resourceArray;
        }
    }
//    footer: Text {
//        text: 'footer'
//    }
}
