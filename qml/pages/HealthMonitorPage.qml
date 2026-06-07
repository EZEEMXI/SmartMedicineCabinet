import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    header: ToolBar {
        background: Rectangle { color: Theme.colorPrimary }
        contentHeight: 56
        Label {
            anchors.centerIn: parent
            text: "心率血氧监测"; font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // ── 连接状态条 ────────────────────────
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 36; radius: 8
            color: App.mqttConnected ? "#dcfce7" : "#fef2f2"
            Label {
                anchors.centerIn: parent
                text: App.mqttConnected
                      ? "● MQTT 已连接 — 接收设备实时数据"
                      : "○ MQTT 未连接 — 等待设备上报"
                font.pixelSize: Theme.fsXS
                color: App.mqttConnected ? "#16a34a" : "#dc2626"
            }
        }

        // ── 心率大卡片 ────────────────────────
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 180
            radius: 16; color: "#fef2f2"
            border.color: "#fecaca"
            border.width: 1

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 8
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "❤️ 心率"; font.pixelSize: Theme.fsMD
                    color: Theme.colorDanger; font.bold: true
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: App.heartRateText; font.pixelSize: 64; font.bold: true
                    color: Theme.colorDanger
                    font.family: "monospace"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "bpm"; font.pixelSize: Theme.fsSM; color: "#ef4444"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "正常范围 60-100 bpm"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
                }
            }

            // 心跳动画
            SequentialAnimation {
                running: App.heartRate > 0
                loops: Animation.Infinite
                NumberAnimation {
                    target: heartPulse; property: "scale"
                    from: 1.0; to: 1.08; duration: 600; easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: heartPulse; property: "scale"
                    from: 1.08; to: 1.0; duration: 400; easing.type: Easing.InOutQuad
                }
            }
        }

        // Transform target (invisible)
        Item { id: heartPulse; visible: false }

        // ── 血氧大卡片 ────────────────────────
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 180
            radius: 16; color: "#eff6ff"
            border.color: "#bfdbfe"
            border.width: 1

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 8
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "🫁 血氧饱和度"; font.pixelSize: Theme.fsMD
                    color: Theme.colorPrimary; font.bold: true
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: App.spo2Text; font.pixelSize: 64; font.bold: true
                    color: Theme.colorPrimary
                    font.family: "monospace"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "SpO₂"; font.pixelSize: Theme.fsSM; color: Theme.colorPrimary
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "正常范围 95%-100%"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
                }
            }
        }

        // ── 药柜状态 ──────────────────────────
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 80
            radius: 12; color: Theme.colorCard

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                // 柜1
                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter; spacing: 4
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: "🗄️ 1号柜"; font.pixelSize: Theme.fsSM; color: Theme.colorText
                    }
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 12; height: 12; radius: 6
                        color: App.cabinet1Status ? Theme.colorAccent : Theme.colorSubText
                    }
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: App.cabinet1Status ? "已打开" : "已关闭"
                        font.pixelSize: Theme.fsXS
                        color: App.cabinet1Status ? Theme.colorAccent : Theme.colorSubText
                    }
                }

                Item { Layout.fillWidth: true }

                // 柜2
                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter; spacing: 4
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: "🗄️ 2号柜"; font.pixelSize: Theme.fsSM; color: Theme.colorText
                    }
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 12; height: 12; radius: 6
                        color: App.cabinet2Status ? Theme.colorAccent : Theme.colorSubText
                    }
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: App.cabinet2Status ? "已打开" : "已关闭"
                        font.pixelSize: Theme.fsXS
                        color: App.cabinet2Status ? Theme.colorAccent : Theme.colorSubText
                    }
                }
            }
        }

        // ── 底部提示 ──────────────────────────
        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "数据通过 MQTT 实时接收 · 由药柜设备上报"
            font.pixelSize: Theme.fsXS; color: Theme.colorSubText
        }
    }
}
