#include "HeatmapWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <algorithm>
#include <cmath>

HeatmapWidget::HeatmapWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(500, 350);
    setAutoFillBackground(true);
}

void HeatmapWidget::setData(const HeatmapData& data)
{
    m_data = data;
    rebuildImage();
    update();
}

void HeatmapWidget::clear()
{
    m_data = HeatmapData{};
    m_heatmapImage = QImage{};
    update();
}

bool HeatmapWidget::savePng(const QString& filePath, QSize imageSize)
{
    QImage output(imageSize, QImage::Format_ARGB32);
    output.fill(Qt::white);

    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing, true);
    drawHeatmap(painter, output.rect(), true);
    painter.end();

    return output.save(filePath, "PNG");
}

void HeatmapWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    drawHeatmap(painter, rect(), false);
}

void HeatmapWidget::rebuildImage()
{
    if (!m_data.valid || m_data.size <= 0) {
        m_heatmapImage = QImage{};
        return;
    }

    const int n = m_data.size;
    m_heatmapImage = QImage(n, n, QImage::Format_RGB32);

    const double span = m_data.maxValue - m_data.minValue;
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            const int idx = row * n + col;
            double t = 0.5;
            if (std::abs(span) > std::numeric_limits<double>::epsilon()) {
                t = (m_data.value[idx] - m_data.minValue) / span;
            }
            t = std::clamp(t, 0.0, 1.0);

            // Draw row 0 at the top. Use mirrored Y if your measurement convention needs it.
            m_heatmapImage.setPixelColor(col, row, rainbowColor(t));
        }
    }
}

void HeatmapWidget::drawHeatmap(QPainter& painter, const QRect& fullRect, bool highQuality) const
{
    painter.save();
    if (highQuality) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
    }

    const int left = 70;
    const int right = 95;
    const int top = 45;
    const int bottom = 60;

    QRect plotRect = fullRect.adjusted(left, top, -right, -bottom);

    painter.setPen(Qt::black);
    QFont titleFont = painter.font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(fullRect.left(), fullRect.top() + 5, fullRect.width(), 30),
                     Qt::AlignCenter, "Heatmap for Regular Grid Data");

    QFont normalFont = painter.font();
    normalFont.setBold(false);
    normalFont.setPointSize(std::max(8, normalFont.pointSize() - 2));
    painter.setFont(normalFont);

    if (!m_data.valid || m_heatmapImage.isNull()) {
        painter.drawRect(plotRect);
        painter.drawText(plotRect, Qt::AlignCenter, "File is not selected");
        painter.restore();
        return;
    }

    painter.drawImage(plotRect, m_heatmapImage);
    painter.drawRect(plotRect);

    const double xMin = *std::min_element(m_data.x.cbegin(), m_data.x.cend());
    const double xMax = *std::max_element(m_data.x.cbegin(), m_data.x.cend());
    const double yMin = *std::min_element(m_data.y.cbegin(), m_data.y.cend());
    const double yMax = *std::max_element(m_data.y.cbegin(), m_data.y.cend());

    painter.drawText(QRect(plotRect.left(), plotRect.bottom() + 25, plotRect.width(), 25),
                     Qt::AlignCenter, "X-axis");

    painter.save();
    painter.translate(fullRect.left() + 18, plotRect.center().y());
    painter.rotate(-90);
    painter.drawText(QRect(-plotRect.height() / 2, -15, plotRect.height(), 25),
                     Qt::AlignCenter, "Y-axis");
    painter.restore();

    painter.drawText(plotRect.left() - 15, plotRect.bottom() + 18, QString::number(xMin, 'g', 5));
    painter.drawText(plotRect.right() - 45, plotRect.bottom() + 18, QString::number(xMax, 'g', 5));
    painter.drawText(plotRect.left() - 60, plotRect.bottom(), QString::number(yMin, 'g', 5));
    painter.drawText(plotRect.left() - 60, plotRect.top() + 8, QString::number(yMax, 'g', 5));

    const QRect colorBar(plotRect.right() + 35, plotRect.top(), 22, plotRect.height());
    for (int y = 0; y < colorBar.height(); ++y) {
        const double t = 1.0 - static_cast<double>(y) / std::max(1, colorBar.height() - 1);
        painter.setPen(rainbowColor(t));
        painter.drawLine(colorBar.left(), colorBar.top() + y, colorBar.right(), colorBar.top() + y);
    }
    painter.setPen(Qt::black);
    painter.drawRect(colorBar);
    painter.drawText(colorBar.right() + 6, colorBar.top() + 8, QString::number(m_data.maxValue, 'g', 5));
    painter.drawText(colorBar.right() + 6, colorBar.bottom(), QString::number(m_data.minValue, 'g', 5));

    painter.save();
    painter.translate(colorBar.right() + 45, colorBar.center().y());
    painter.rotate(-90);
    painter.drawText(QRect(-colorBar.height() / 2, -15, colorBar.height(), 25),
                     Qt::AlignCenter, "Magnitude");
    painter.restore();

    painter.restore();
}

QColor HeatmapWidget::rainbowColor(double t)
{
    // Approximation of matplotlib's rainbow-like map using HSV hue sweep.
    // t=0 -> violet/blue, t=1 -> red.
    t = std::clamp(t, 0.0, 1.0);
    const double hue = (1.0 - t) * 270.0 / 360.0;
    QColor c;
    c.setHsvF(hue, 1.0, 1.0);
    return c;
}
