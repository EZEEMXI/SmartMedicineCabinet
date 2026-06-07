#ifndef THEME_H
#define THEME_H

#include <QObject>

/**
 * 全局主题 — 统一管理颜色与字号，供所有 QML 文件使用
 *
 * 替代 main.qml 中 ApplicationWindow 的 readonly property，
 * 解决独立 .qml 文件无法访问 main.qml root id 的问题。
 */
class Theme : public QObject
{
    Q_OBJECT

    // ── 颜色 ──────────────────────────────────
    Q_PROPERTY(QString colorBg       READ colorBg      CONSTANT)
    Q_PROPERTY(QString colorCard     READ colorCard    CONSTANT)
    Q_PROPERTY(QString colorPrimary  READ colorPrimary CONSTANT)
    Q_PROPERTY(QString colorAccent   READ colorAccent  CONSTANT)
    Q_PROPERTY(QString colorWarning  READ colorWarning CONSTANT)
    Q_PROPERTY(QString colorDanger   READ colorDanger  CONSTANT)
    Q_PROPERTY(QString colorText     READ colorText    CONSTANT)
    Q_PROPERTY(QString colorSubText  READ colorSubText CONSTANT)
    Q_PROPERTY(QString colorBorder   READ colorBorder  CONSTANT)

    // ── 字体大小 ──────────────────────────────
    Q_PROPERTY(int fsXL READ fsXL CONSTANT)
    Q_PROPERTY(int fsLG READ fsLG CONSTANT)
    Q_PROPERTY(int fsMD READ fsMD CONSTANT)
    Q_PROPERTY(int fsSM READ fsSM CONSTANT)
    Q_PROPERTY(int fsXS READ fsXS CONSTANT)

public:
    explicit Theme(QObject *parent = nullptr);

    QString colorBg()      const { return "#f0f2f5"; }
    QString colorCard()    const { return "#ffffff"; }
    QString colorPrimary() const { return "#2563eb"; }
    QString colorAccent()  const { return "#10b981"; }
    QString colorWarning() const { return "#f59e0b"; }
    QString colorDanger()  const { return "#ef4444"; }
    QString colorText()    const { return "#1a1a2e"; }
    QString colorSubText() const { return "#6b7280"; }
    QString colorBorder()  const { return "#e5e7eb"; }

    int fsXL() const { return 24; }
    int fsLG() const { return 18; }
    int fsMD() const { return 15; }
    int fsSM() const { return 13; }
    int fsXS() const { return 11; }
};

#endif // THEME_H
