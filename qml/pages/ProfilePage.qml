import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    // 供外部调用：回到"我的"根页面
    function popToRoot() {
        if (profileStack.depth > 1)
            profileStack.pop(null)
    }

    // ── 内部 StackView ───
    StackView {
        id: profileStack
        anchors.fill: parent

        // 返回键：优先 pop 内部栈
        Keys.onBackPressed: function(event) {
            if (depth > 1) {
                pop()
                event.accepted = true
            }
        }

        initialItem: Item {
            width: profileStack.width
            height: profileStack.height

            ScrollView {
                anchors.fill: parent
                contentWidth: availableWidth

                ColumnLayout {
                    width: parent.width; spacing: 0

                    // ── 头像卡片 ────────────────────────
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 100
                        color: Theme.colorCard
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 16
                            Rectangle {
                                width: 60; height: 60; radius: 30; color: Theme.colorPrimary
                                Label { anchors.centerIn: parent; text: "👤"; font.pixelSize: 28 }
                            }
                            ColumnLayout { spacing: 4
                                Label {
                                    text: App.userName; font.pixelSize: Theme.fsLG; font.bold: true; color: Theme.colorText
                                }
                                RowLayout { spacing: 6
                                    Label { text: "智慧药柜用户"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                                    Rectangle {
                                        visible: App.isAdmin
                                        radius: 4; color: Theme.colorWarning
                                        width: adminTag.implicitWidth + 8; height: 18
                                        Label {
                                            id: adminTag
                                            anchors.centerIn: parent
                                            text: "管理员"; font.pixelSize: 10; font.bold: true; color: "#ffffff"
                                        }
                                    }
                                }
                            }
                            Item { Layout.fillWidth: true }
                            Label { text: "›"; font.pixelSize: Theme.fsLG; color: Theme.colorSubText }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: profileStack.push("qrc:/qml/pages/PersonalInfoPage.qml")
                        }
                    }

                    Item { Layout.preferredHeight: 8 }

                    // ── 菜单项 ──────────────────────────
                    Repeater {
                        model: menuModel
                        delegate: Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 50; color: Theme.colorCard
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 12
                                Label { text: model.icon; font.pixelSize: Theme.fsMD }
                                Label {
                                    text: model.label; font.pixelSize: Theme.fsMD; color: Theme.colorText
                                    Layout.fillWidth: true
                                }
                                Label { text: "›"; font.pixelSize: Theme.fsLG; color: Theme.colorSubText }
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (model.action === "orders")
                                        profileStack.push("qrc:/qml/pages/OrderPage.qml")
                                    else if (model.action === "health")
                                        profileStack.push("qrc:/qml/pages/HealthMonitorPage.qml")
                                    else if (model.action === "news")
                                        profileStack.push("qrc:/qml/pages/HealthNewsPage.qml")
                                }
                            }
                            Rectangle {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                height: 1; color: Theme.colorBorder
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 8 }

                    // ── MQTT 状态 ───────────────────────
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 50; color: Theme.colorCard
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            Label { text: "☁️"; font.pixelSize: Theme.fsMD }
                            Label {
                                text: "云平台状态"; font.pixelSize: Theme.fsMD; color: Theme.colorText
                                Layout.fillWidth: true
                            }
                            Rectangle {
                                width: 10; height: 10; radius: 5
                                color: App.mqttConnected ? "#16a34a" : "#9ca3af"
                            }
                        }
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: 1; color: Theme.colorBorder
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 44; color: Theme.colorCard
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            Item { Layout.preferredWidth: 24 }
                            Label {
                                text: App.mqttStatus; font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                                Layout.fillWidth: true
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 16 }

                    // ── 连接按钮 ────────────────────────
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 48
                        Layout.margins: 16; radius: 12
                        color: App.mqttConnected ? "#fef2f2" : "#dcfce7"
                        Label {
                            anchors.centerIn: parent
                            text: App.mqttConnected ? "断开云平台连接" : "连接云平台"
                            font.pixelSize: Theme.fsSM; font.bold: true
                            color: App.mqttConnected ? Theme.colorDanger : Theme.colorAccent
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (App.mqttConnected) App.disconnectMqtt()
                                else App.connectMqtt()
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 40 }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: "智能药柜 v1.0 · Qt6"
                        font.pixelSize: Theme.fsXS; color: Theme.colorSubText
                    }
                    Item { Layout.preferredHeight: 20 }
                }
            }
        }
    }

    // ── 菜单数据 ──────────────────────────────
    ListModel {
        id: menuModel
        ListElement { icon: "📋"; label: "购药记录"; action: "orders" }
        ListElement { icon: "💚"; label: "心率血氧监测"; action: "health" }
        ListElement { icon: "📰"; label: "健康资讯"; action: "news" }
    }
}
