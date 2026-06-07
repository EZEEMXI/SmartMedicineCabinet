import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    header: ToolBar {
        background: Rectangle { color: Theme.colorPrimary }
        contentHeight: 56
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 4; anchors.rightMargin: 16
            ToolButton {
                text: "\u2190"; font.pixelSize: Theme.fsLG; font.bold: true
                onClicked: page.StackView.view.pop()
                contentItem: Label {
                    text: "\u2190"; font: parent.font; color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
            }
            Label {
                text: "个人信息"; font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
            }
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: col.implicitHeight
        clip: true

        ColumnLayout {
            id: col
            width: parent.width
            spacing: 0

            // ── 头像 ────────────────────────────
            Item { Layout.preferredHeight: 28 }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 88; height: 88; radius: 44
                color: Theme.colorPrimary
                Label {
                    anchors.centerIn: parent
                    text: App.isAdmin ? "\uD83D\uDC68\u200D\u2695\uFE0F" : "\uD83D\uDC64"
                    font.pixelSize: 40
                }
            }

            Item { Layout.preferredHeight: 16 }

            // ── 信息卡片 ────────────────────────
            Rectangle {
                id: infoCard
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.preferredHeight: infoCol.implicitHeight
                radius: 12; color: Theme.colorCard

                ColumnLayout {
                    id: infoCol
                    width: parent.width
                    spacing: 0

                    // 昵称
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        Layout.leftMargin: 16; Layout.rightMargin: 16
                        Label {
                            text: "昵称"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            text: tfNickname.text
                            onTextChanged: tfNickname.text = text
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        Layout.leftMargin: 72
                        color: Theme.colorBorder
                    }

                    // 手机号
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        Layout.leftMargin: 16; Layout.rightMargin: 16
                        Label {
                            text: "手机号"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            text: tfPhone.text
                            onTextChanged: tfPhone.text = text
                            validator: RegularExpressionValidator { regularExpression: /^[0-9]{0,11}$/ }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        Layout.leftMargin: 72
                        color: Theme.colorBorder
                    }

                    // 地址
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        Layout.leftMargin: 16; Layout.rightMargin: 16
                        Label {
                            text: "地  址"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            text: tfAddress.text
                            onTextChanged: tfAddress.text = text
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 24 }

            // ── 保存按钮 ────────────────────────
            Button {
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.preferredHeight: 44
                background: Rectangle { radius: 12; color: Theme.colorPrimary }
                contentItem: Label {
                    text: "保存"; font.pixelSize: Theme.fsMD; font.bold: true
                    color: "#fff"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    App.userName = tfNickname.text
                    App.saveUserInfo(tfPhone.text, tfAddress.text)
                    toast.show("已保存")
                }
            }

            Item { Layout.preferredHeight: 20 }
        }
    }

    // ── 隐藏的输入字段（数据源）─────────────────
    TextField {
        id: tfNickname
        visible: false
        text: App.userName === "用户" ? "" : App.userName
    }
    TextField {
        id: tfPhone
        visible: false
        text: App.userPhone
        validator: RegularExpressionValidator { regularExpression: /^[0-9]{0,11}$/ }
    }
    TextField {
        id: tfAddress
        visible: false
        text: App.userAddress
    }

    // ── Toast ──────────────────────────────────
    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        width: toastLabel.implicitWidth + 32; height: 40; radius: 20
        color: "#333"; opacity: 0; visible: false
        function show(msg) { toastLabel.text = msg; opacity = 1; visible = true; toastTimer.restart() }
        Label { id: toastLabel; anchors.centerIn: parent; color: "#fff"; font.pixelSize: Theme.fsSM }
        Timer { id: toastTimer; interval: 1800; onTriggered: { toast.opacity = 0; toast.visible = false } }
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }
}
