import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 420
    height: 780
    title: "智能药柜"

    // ── 白色主题 ────────────────────────────
    readonly property color colorBg:       "#f0f2f5"
    readonly property color colorCard:     "#ffffff"
    readonly property color colorPrimary:  "#2563eb"
    readonly property color colorAccent:   "#10b981"
    readonly property color colorWarning:  "#f59e0b"
    readonly property color colorDanger:   "#ef4444"
    readonly property color colorText:     "#1a1a2e"
    readonly property color colorSubText:  "#6b7280"
    readonly property color colorBorder:   "#e5e7eb"

    // ── 全局字体大小 ────────────────────────
    readonly property int fsXL:  24
    readonly property int fsLG:  18
    readonly property int fsMD:  15
    readonly property int fsSM:  13
    readonly property int fsXS:  11

    // ── StackView ───────────────────────────
    StackView {
        id: stack
        anchors.fill: parent
        initialItem: "qrc:/qml/pages/LoginPage.qml"

        // ── 回到登录页（退出登录）──
        function backToLogin() {
            var d = depth
            for (var i = d - 1; i > 0; i--)
                pop(null)
        }

        Keys.onBackPressed: function(event) {
            if (depth > 1) {
                pop()
                event.accepted = true
            } else {
                // 已经在根页面 → 显示退出确认
                exitDialog.open()
                event.accepted = true
            }
        }

        pushEnter: Transition {
            NumberAnimation { property: "x"; from: stack.width; to: 0; duration: 250; easing.type: Easing.OutCubic }
        }
        pushExit: Transition {
            NumberAnimation { property: "x"; from: 0; to: -stack.width * 0.3; duration: 200; easing.type: Easing.InCubic }
        }
        popEnter: Transition {
            NumberAnimation { property: "x"; from: -stack.width * 0.3; to: 0; duration: 200; easing.type: Easing.OutCubic }
        }
        popExit: Transition {
            NumberAnimation { property: "x"; from: 0; to: stack.width; duration: 250; easing.type: Easing.InCubic }
        }
    }

    // ── 退出确认弹窗 ─────────────────────────
    Dialog {
        id: exitDialog
        title: "确认退出"
        standardButtons: Dialog.No | Dialog.Yes
        anchors.centerIn: parent
        width: 300
        modal: true

        background: Rectangle {
            radius: 16; color: Theme.colorCard
            border.color: Theme.colorBorder
        }

        header: Label {
            text: "确认退出"
            font.pixelSize: Theme.fsLG; font.bold: true
            color: Theme.colorText
            padding: 20
        }

        contentItem: Label {
            text: "确定要退出智能药柜 APP 吗？"
            font.pixelSize: Theme.fsMD
            color: Theme.colorSubText
            horizontalAlignment: Text.AlignHCenter
            padding: 12
        }

        footer: RowLayout {
            spacing: 12
            Layout.fillWidth: true
            Layout.margins: 16

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                text: "取消"
                font.pixelSize: Theme.fsSM
                background: Rectangle { radius: 10; color: "#e5e7eb" }
                contentItem: Label { text: "取消"; font.pixelSize: Theme.fsSM; color: Theme.colorText; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: exitDialog.close()
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                text: "退出"
                font.pixelSize: Theme.fsSM; font.bold: true
                background: Rectangle { radius: 10; color: Theme.colorDanger }
                contentItem: Label { text: "退出"; font.pixelSize: Theme.fsSM; font.bold: true; color: "#fff"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: Qt.quit()
            }
        }
    }
}
