#pragma once

#include <QString>
#include <QVector>

struct HeatmapData
{
    int size = 0;
    QVector<double> x;
    QVector<double> y;
    QVector<double> value;
    double minValue = 0.0;
    double maxValue = 0.0;
    bool valid = false;
    QString error;
};

HeatmapData loadHeatmapData(const QString& filePath);
