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
    header: Label {
        padding: 6
        horizontalAlignment: Text.AlignHCenter
        color: 'gray'
        text: invalid ? '' : error ? qsTr('error') : loading ? qsTr('loading') : endpoint.url
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
                Layout.minimumWidth: root.width / 4
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
                            root.endpoint = modelData
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
                                    id: todoPropertyInfo
                                    Text {
                                        text: 'todo'
                                    }
                                }
                                Component {
                                    id: noPropertyInfo
                                    Text {
                                        text: 'select something'
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
                                        model: selectedResourceProperty.resources
                                        delegate: ItemDelegate {
                                            text: modelData.name
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
        Text {
            text: 'resources: ' + endpoint.resources.length
        }
    }
    Loader {
        anchors.fill: parent
        sourceComponent: !root.loaded ? loadingPage
                                      : root.singleEntity ? singleResource
                                                          : resourceArray
    }
    footer: Text {
        text: 'footer'
    }
}
