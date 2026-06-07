import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    property string drugName: ""
    property double drugPrice: 0
    property string drugEmoji: ""
    property string drugColor: "#2563eb"
    property string drugDesc: ""
    property string drugUsage: ""
    property string drugWarning: ""

    header: ToolBar {
        background: Rectangle { color: drugColor }
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 4
            anchors.rightMargin: 16
            ToolButton {
                text: "←"
                font.pixelSize: Theme.fsLG; font.bold: true
                onClicked: page.StackView.view.pop()
                contentItem: Label {
                    text: "←"; font: parent.font; color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Label {
                text: drugName
                font.pixelSize: Theme.fsMD; font.bold: true; color: "#ffffff"
                elide: Text.ElideRight
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width; spacing: 0

            // ── 大图标 ──────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 160
                color: Qt.lighter(drugColor, 1.8)
                Label {
                    anchors.centerIn: parent
                    text: drugEmoji; font.pixelSize: 64
                }
            }

            // ── 名称价格 ────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 70
                color: Theme.colorCard
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    Label {
                        text: drugName
                        font.pixelSize: Theme.fsXL; font.bold: true
                        color: Theme.colorText
                        Layout.fillWidth: true
                    }
                    Label {
                        text: "¥" + drugPrice.toFixed(2)
                        font.pixelSize: Theme.fsXL; font.bold: true
                        color: Theme.colorDanger
                    }
                }
            }

            Item { Layout.preferredHeight: 8 }

            // ── 适应症 ──────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                radius: 12; color: Theme.colorCard
                Layout.preferredHeight: section1Inner.implicitHeight + 20
                ColumnLayout {
                    id: section1Inner
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    Label {
                        text: "📋 适应症"
                        font.pixelSize: Theme.fsMD; font.bold: true
                        color: Theme.colorText
                    }
                    Label {
                        text: drugDesc
                        font.pixelSize: Theme.fsSM
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        lineHeight: 1.6
                    }
                }
            }

            // ── 用法用量 ────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                radius: 12; color: Theme.colorCard
                Layout.preferredHeight: section2Inner.implicitHeight + 20
                ColumnLayout {
                    id: section2Inner
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    Label {
                        text: "💡 用法用量"
                        font.pixelSize: Theme.fsMD; font.bold: true
                        color: Theme.colorText
                    }
                    Label {
                        text: drugUsage
                        font.pixelSize: Theme.fsSM
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        lineHeight: 1.6
                    }
                }
            }

            // ── 注意事项 ────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                radius: 12; color: "#fef3c7"
                Layout.preferredHeight: colWarn.implicitHeight + 24

                ColumnLayout {
                    id: colWarn
                    anchors.fill: parent
                    anchors.margins: 12
                    Label {
                        text: "⚠️ 注意事项"
                        font.pixelSize: Theme.fsMD; font.bold: true
                        color: Theme.colorWarning
                    }
                    Label {
                        text: drugWarning
                        font.pixelSize: Theme.fsSM
                        color: "#92400e"
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            Item { Layout.preferredHeight: 16 }
        }
    }

    // ── 底部加入购物车 ────────────────────────
    footer: Rectangle {
        width: parent.width; height: 64
        color: Theme.colorCard
        border.color: Theme.colorBorder
        border.width: 1

        Button {
            anchors.centerIn: parent
            width: parent.width - 48; height: 44
            background: Rectangle {
                radius: 12; color: parent.hovered ? "#059669" : Theme.colorAccent
            }
            contentItem: Label {
                text: "加入购物车  ¥" + drugPrice.toFixed(2)
                font.pixelSize: Theme.fsMD; font.bold: true
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                App.addToCart(drugName, drugPrice)
                toast.show("已加入购物车：" + drugName)
            }
        }
    }

    // ── Toast ──────────────────────────────────
    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80
        width: toastLabel.implicitWidth + 32; height: 40; radius: 20
        color: "#333333"; opacity: 0; visible: false

        function show(msg) {
            toastLabel.text = msg; opacity = 1; visible = true
            toastTimer.restart()
        }

        Label {
            id: toastLabel
            anchors.centerIn: parent
            color: "#ffffff"; font.pixelSize: Theme.fsSM
        }

        Timer {
            id: toastTimer
            interval: 1800; repeat: false
            onTriggered: { toast.opacity = 0; toast.visible = false }
        }

        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

}
