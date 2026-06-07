import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    // 切换 tab 的辅助函数（必须放在顶层）
    function gotoPage(idx) {
        var sv = page.parent       // Loader
        if (sv) sv = sv.parent     // SwipeView (contentView)
        if (sv) sv.currentIndex = idx
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: 0

            // ── 欢迎头部 ─────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                color: Theme.colorPrimary

                ColumnLayout {
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 8

                    Label {
                        text: "你好，" + App.userName
                        font.pixelSize: Theme.fsXL; font.bold: true; color: "#ffffff"
                    }
                    Label {
                        text: "智能药柜 · 云端管理 · 实时监控"
                        font.pixelSize: Theme.fsSM; color: "#bfdbfe"
                    }
                }
            }

            Item { Layout.preferredHeight: 16 }

            // ── 摘要卡片行 ───────────────────────
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 16
                spacing: 12

                // 云平台状态
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 76
                    radius: 10; color: Theme.colorCard
                    ColumnLayout {
                        anchors.centerIn: parent; spacing: 4
                        Label { Layout.alignment: Qt.AlignHCenter; text: "\u2601\uFE0F"; font.pixelSize: 22 }
                        Label { Layout.alignment: Qt.AlignHCenter; text: "云平台"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: App.mqttConnected ? "已连接" : "未连接"
                            font.pixelSize: Theme.fsSM; font.bold: true
                            color: App.mqttConnected ? "#16a34a" : "#9ca3af"
                        }
                    }
                }

                // 药柜状态
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 76
                    radius: 10; color: Theme.colorCard
                    ColumnLayout {
                        anchors.centerIn: parent; spacing: 4
                        Label { Layout.alignment: Qt.AlignHCenter; text: "\uD83D\uDCE6"; font.pixelSize: 22 }
                        Label { Layout.alignment: Qt.AlignHCenter; text: "药柜"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: (App.cabinet1Status || App.cabinet2Status) ? "已开启" : "已关闭"
                            font.pixelSize: Theme.fsSM; font.bold: true
                            color: (App.cabinet1Status || App.cabinet2Status) ? Theme.colorAccent : "#9ca3af"
                        }
                    }
                }

                // 购物车
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 76
                    radius: 10; color: Theme.colorCard
                    ColumnLayout {
                        anchors.centerIn: parent; spacing: 4
                        Label { Layout.alignment: Qt.AlignHCenter; text: "\uD83D\uDED2"; font.pixelSize: 22 }
                        Label { Layout.alignment: Qt.AlignHCenter; text: "购物车"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: App.cartCount > 0 ? App.cartCount + "件" : "空"
                            font.pixelSize: Theme.fsSM; font.bold: true
                            color: App.cartCount > 0 ? Theme.colorPrimary : "#9ca3af"
                        }
                    }
                }
            }

            // ── 健康数据 ─────────────────────────
            Item { Layout.preferredHeight: 4 }
            Label {
                Layout.leftMargin: 20
                text: "健康监测"; font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorText
            }
            Item { Layout.preferredHeight: 8 }

            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 16
                spacing: 12

                // 心率卡片
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 100
                    radius: 12; color: "#fee2e2"
                    ColumnLayout {
                        anchors.centerIn: parent; spacing: 4
                        Label { Layout.alignment: Qt.AlignHCenter; text: "\u2764\uFE0F"; font.pixelSize: 24 }
                        Label { Layout.alignment: Qt.AlignHCenter; text: "心率"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            Label { text: App.heartRateText; font.pixelSize: 36; font.bold: true; color: Theme.colorDanger }
                            Label { text: " bpm"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText }
                        }
                    }
                }

                // 血氧卡片
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 100
                    radius: 12; color: "#dbeafe"
                    ColumnLayout {
                        anchors.centerIn: parent; spacing: 4
                        Label { Layout.alignment: Qt.AlignHCenter; text: "\uD83E\uDEB1"; font.pixelSize: 24 }
                        Label { Layout.alignment: Qt.AlignHCenter; text: "血氧"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            Label { text: App.spo2Text; font.pixelSize: 36; font.bold: true; color: Theme.colorPrimary }
                            Label { text: " %"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText }
                        }
                    }
                }
            }

            // ── 快捷入口 ─────────────────────────
            Item { Layout.preferredHeight: 16 }
            Label {
                Layout.leftMargin: 20
                text: "快捷操作"; font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorText
            }
            Item { Layout.preferredHeight: 8 }

            // 查看药品
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 16
                Layout.preferredHeight: 60
                radius: 12; color: Theme.colorCard
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    Label { text: "\uD83D\uDC8A"; font.pixelSize: 24 }
                    ColumnLayout {
                        spacing: 2; Layout.fillWidth: true
                        Label { text: "药品货架"; font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorText }
                        Label { text: "浏览和购买药品"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                    }
                    Label { text: "\u203A"; font.pixelSize: 24; color: Theme.colorSubText }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: gotoPage(1)
                }
            }

            // 查看健康
            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.preferredHeight: 60
                radius: 12; color: Theme.colorCard
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    Label { text: "\u2764\uFE0F"; font.pixelSize: 24 }
                    ColumnLayout {
                        spacing: 2; Layout.fillWidth: true
                        Label { text: "健康监测"; font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorText }
                        Label { text: "心率血氧实时数据"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText }
                    }
                    Label { text: "\u203A"; font.pixelSize: 24; color: Theme.colorSubText }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: gotoPage(2)
                }
            }

            Item { Layout.preferredHeight: 40 }
        }
    }
}
