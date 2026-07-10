#include "MainWindow.h"
#include "HeatmapData.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Heatmap viewer");
    resize(900, 500);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    auto* leftFrame = new QWidget(splitter);
    auto* leftLayout = new QVBoxLayout(leftFrame);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    leftLayout->setSpacing(10);

    auto* title = new QLabel("Control Panel", leftFrame);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(title);

    auto* selectButton = new QPushButton("Select a file", leftFrame);
    connect(selectButton, &QPushButton::clicked, this, &MainWindow::chooseFile);
    leftLayout->addWidget(selectButton);

    auto* saveButton = new QPushButton("Save PNG", leftFrame);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveCurrentPng);
    leftLayout->addWidget(saveButton);

    m_folderLabel = new QLabel("Folder is not selected", leftFrame);
    m_folderLabel->setWordWrap(true);
    leftLayout->addWidget(m_folderLabel);

    m_fileLabel = new QLabel("File is not selected", leftFrame);
    m_fileLabel->setWordWrap(true);
    leftLayout->addWidget(m_fileLabel);

    leftLayout->addStretch(1);

    m_heatmap = new HeatmapWidget(splitter);
    splitter->addWidget(leftFrame);
    splitter->addWidget(m_heatmap);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
}

void MainWindow::chooseFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select a file",
        QString(),
        "Data files (*.txt *.csv);;Text files (*.txt);;CSV files (*.csv);;All files (*.*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    openFile(filePath);
}

void MainWindow::openFile(const QString& filePath)
{
    const HeatmapData data = loadHeatmapData(filePath);
    if (!data.valid) {
        QMessageBox::critical(this, "Cannot load heatmap", data.error);
        return;
    }

    m_currentFile = filePath;
    QFileInfo info(filePath);
    m_folderLabel->setText(info.absolutePath());
    m_fileLabel->setText(info.fileName());
    m_heatmap->setData(data);
}

void MainWindow::saveCurrentPng()
{
    if (m_currentFile.isEmpty()) {
        QMessageBox::information(this, "No data", "Select a file first.");
        return;
    }

    const QString defaultName = QFileInfo(m_currentFile).absolutePath() + "/" +
                                QFileInfo(m_currentFile).completeBaseName() + ".png";

    const QString outputPath = QFileDialog::getSaveFileName(
        this,
        "Save PNG",
        defaultName,
        "PNG image (*.png)"
    );

    if (outputPath.isEmpty()) {
        return;
    }

    if (!m_heatmap->savePng(outputPath)) {
        QMessageBox::critical(this, "Save failed", "Cannot save PNG file.");
    }
}
