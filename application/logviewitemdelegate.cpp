// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewitemdelegate.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DPalette>
#include <DStyle>
#include <DStyleHelper>

#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QPainterPath>

DWIDGET_USE_NAMESPACE

LogViewItemDelegate::LogViewItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}
/**
 * @brief LogViewItemDelegate::paint 绘制内容数据和文字虚函数
 * @param painter
 * @param option
 * @param index
 */
void LogViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(1);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!(opt.state & DStyle::State_Enabled)) {
        cg = DPalette::Disabled;
    } else {
        if (!wnd) {
            cg = DPalette::Inactive;
        } else {
            cg = DPalette::Active;
        }
    }
    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);
    //设置高亮文字色
    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
    QPen forground;
    forground.setColor(palette.color(cg, DPalette::Text));
    if (opt.state & DStyle::State_Enabled) {
        if (opt.state & DStyle::State_Selected) {
            forground.setColor(palette.color(cg, DPalette::HighlightedText));
        }
    }
    painter->setPen(forground);
    QRect rect = opt.rect;
    QFontMetrics fm(opt.font);
    QPainterPath path, clipPath;
    QRect textRect = rect;
    switch (opt.viewItemPosition) {
    case QStyleOptionViewItem::Beginning: {
        // 左间距
        rect.setX(rect.x() + margin);
    } break;
    case QStyleOptionViewItem::Middle: {
    } break;
    case QStyleOptionViewItem::End: {
        // 右间距
        rect.setWidth(rect.width() - margin);
    } break;
    case QStyleOptionViewItem::OnlyOne: {
        // 左间距
        rect.setX(rect.x() + margin);
        // 右间距
        rect.setWidth(rect.width() - margin);
    } break;
    default: {
        painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    }
    //绘制图标
    QRect iconRect = rect;
    if (opt.viewItemPosition == QStyleOptionViewItem::Beginning &&
            index.data(Qt::DecorationRole).isValid()) {
        iconRect.setWidth(36);
        iconRect.setHeight(36);
        iconRect.setX(rect.x() + margin);
        QIcon ic = index.data(Qt::DecorationRole).value<QIcon>();
        ic.paint(painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter);
    }
    //绘制文字
    textRect = rect;
    textRect.setX(textRect.x() + margin - 2);
    QString text = fm.elidedText(opt.text, opt.textElideMode, textRect.width());
    painter->drawText(textRect, Qt::TextSingleLine | static_cast<int>(opt.displayAlignment), text);
    painter->restore();
}

QSize LogViewItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(36);
    return size;
}
/**
 * @brief LogViewItemDelegate::initStyleOption 初始化样式，文字省略模式，内容居中居左等等
 * @param option
 * @param index
 */
void LogViewItemDelegate::initStyleOption(QStyleOptionViewItem *option,
                                          const QModelIndex &index) const
{
    option->showDecorationSelected = true;
    bool ok = false;
    if (index.data(Qt::TextAlignmentRole).isValid()) {
        uint value = index.data(Qt::TextAlignmentRole).toUInt(&ok);
        option->displayAlignment = static_cast<Qt::Alignment>(value);
    }

    if (!ok)
        option->displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option->textElideMode = Qt::ElideRight;
    option->features = QStyleOptionViewItem::HasDisplay;
    if (index.row() % 2 == 0)
        option->features |= QStyleOptionViewItem::Alternate;
    if (index.data(Qt::DisplayRole).isValid())
        option->text = index.data().toString();
}
