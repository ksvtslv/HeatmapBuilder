#include "MainWindow.h"
#include "HeatmapData.h"
#include "HeatmapWidget.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

static bool saveOne(const QString& inputPath, const QString& outputPath)
{
    HeatmapData data = loadHeatmapData(inputPath);
    if (!data.valid) {
        qWarning().noquote() << "Cannot process" << inputPath << ":" << data.error;
        return false;
    }

    HeatmapWidget widget;
    widget.setData(data);
    if (!widget.savePng(outputPath)) {
        qWarning().noquote() << "Cannot save" << outputPath;
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("QtHeatmapViewer");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt/C++ heatmap viewer and PNG generator.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption inputOption({"i", "input"}, "Input data file.", "file");
    QCommandLineOption pathOption({"p", "path"}, "Folder with .txt data files.", "folder");
    QCommandLineOption outputOption({"o", "output"}, "Output PNG file or output folder.", "path");
    parser.addOption(inputOption);
    parser.addOption(pathOption);
    parser.addOption(outputOption);
    parser.addPositionalArgument("file", "Optional input file to open in the GUI.");
    parser.process(app);

    const QString input = parser.value(inputOption);
    const QString folder = parser.value(pathOption);
    const QString output = parser.value(outputOption);

    if (!input.isEmpty()) {
        QString outputPath;
        if (!output.isEmpty()) {
            QFileInfo outInfo(output);
            if (outInfo.isDir() || output.endsWith('/') || output.endsWith('\\')) {
                QDir().mkpath(output);
                outputPath = QDir(output).filePath(QFileInfo(input).completeBaseName() + ".png");
            } else {
                QDir().mkpath(outInfo.absolutePath());
                outputPath = output;
            }
        } else {
            outputPath = input + ".png";
        }
        return saveOne(input, outputPath) ? 0 : 1;
    }

    if (!folder.isEmpty()) {
        QDir dir(folder);
        if (!dir.exists()) {
            qWarning().noquote() << "Folder does not exist:" << folder;
            return 1;
        }

        const QString outputFolder = output.isEmpty() ? folder : output;
        QDir().mkpath(outputFolder);

        int failed = 0;
        const QFileInfoList files = dir.entryInfoList({"*.txt"}, QDir::Files);
        for (const QFileInfo& file : files) {
            const QString outputPath = QDir(outputFolder).filePath(file.fileName() + ".png");
            if (!saveOne(file.absoluteFilePath(), outputPath)) {
                ++failed;
            }
        }
        return failed == 0 ? 0 : 1;
    }

    MainWindow window;
    const QStringList positional = parser.positionalArguments();
    if (!positional.isEmpty()) {
        window.openFile(positional.first());
    }
    window.show();
    return app.exec();
}
