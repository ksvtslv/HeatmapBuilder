#pragma once

#include "HeatmapWidget.h"

#include <QMainWindow>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void openFile(const QString& filePath);

private slots:
    void chooseFile();
    void saveCurrentPng();

private:
    HeatmapWidget* m_heatmap = nullptr;
    QLabel* m_folderLabel = nullptr;
    QLabel* m_fileLabel = nullptr;
    QString m_currentFile;
};
