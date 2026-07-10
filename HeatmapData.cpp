#include "HeatmapData.h"

#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <algorithm>
#include <cmath>
#include <limits>

static bool isHeaderLine(const QString& line)
{
    const QString trimmed = line.trimmed();
    return trimmed.startsWith('X') && trimmed.contains('Y') && trimmed.contains("Value");
}

HeatmapData loadHeatmapData(const QString& filePath)
{
    HeatmapData result;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.error = "Cannot open file: " + file.errorString();
        return result;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines << in.readLine();
    }

    int headerIndex = -1;
    for (int i = 0; i < lines.size(); ++i) {
        if (isHeaderLine(lines[i])) {
            headerIndex = i;
            break;
        }
    }

    if (headerIndex < 0) {
        result.error = "Header line with columns X Y Value was not found.";
        return result;
    }

    QVector<double> rawX;
    QVector<double> rawY;
    QVector<double> rawValue;

    for (int i = headerIndex + 1; i < lines.size(); ++i) {
        const QString line = lines[i].trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 3) {
            continue;
        }

        bool okX = false;
        bool okY = false;
        bool okValue = false;
        const double x = parts[0].toDouble(&okX);
        const double y = parts[1].toDouble(&okY);
        const double value = parts[2].toDouble(&okValue);

        if (!okX || !okY || !okValue) {
            continue;
        }

        rawX.push_back(x);
        rawY.push_back(y);
        rawValue.push_back(value);
    }

    if (rawX.isEmpty()) {
        result.error = "No numeric rows were found after the header.";
        return result;
    }

    int count = 0;
    for (int i = 0; i < rawX.size(); ++i) {
        if (rawX[0] < rawX[i]) {
            break;
        }
        ++count;
    }

    if (count <= 0 || rawX.size() != count * count) {
        result.error = QString("Input is not a square regular grid. Rows=%1, detected size=%2.")
                           .arg(rawX.size())
                           .arg(count);
        return result;
    }

    result.size = count;
    result.x.resize(count * count);
    result.y.resize(count * count);
    result.value.resize(count * count);

    for (int row = 0; row < count; ++row) {
        for (int col = 0; col < count; ++col) {
            const int src = row * count + col;
            int dstCol = col;

            // Same serpentine correction as the original Python code:
            // reverse every odd row.
            if (row % 2 == 1) {
                dstCol = count - 1 - col;
            }

            const int dst = row * count + dstCol;
            result.x[dst] = rawX[src];
            result.y[dst] = rawY[src];
            result.value[dst] = rawValue[src];
        }
    }

    const auto [minIt, maxIt] = std::minmax_element(result.value.cbegin(), result.value.cend());
    result.minValue = *minIt;
    result.maxValue = *maxIt;
    result.valid = true;
    return result;
}
