#pragma once

#include "HeatmapData.h"

#include <QWidget>
#include <QImage>

class HeatmapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HeatmapWidget(QWidget* parent = nullptr);

    void setData(const HeatmapData& data);
    void clear();
    bool savePng(const QString& filePath, QSize imageSize = QSize(1200, 900));

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    HeatmapData m_data;
    QImage m_heatmapImage;

    void rebuildImage();
    void drawHeatmap(QPainter& painter, const QRect& rect, bool highQuality) const;
    static QColor rainbowColor(double t);
};
