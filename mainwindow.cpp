#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    res(nullptr)
{
    ui->setupUi(this);
    ui->picInfo->setText("Please open a RES file before proceeding...");
    /* Add actions for the widgets */
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileClicked()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionSaveas, SIGNAL(triggered()), this, SLOT(saveResAs()));
    connect(ui->fileList, SIGNAL(currentRowChanged(int)), this, SLOT(imageSelected(int)));
    connect(ui->picExport, SIGNAL(clicked()), this, SLOT(exportImage()));
    connect(ui->picImport, SIGNAL(clicked()), this, SLOT(importImage()));
    ui->picImport->hide();
    ui->picExport->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileClicked() {
    /* Open a file browser to select the .res file */
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open File..."), "",
                                                    tr("RES Files (*.res)"));

    if (filename.isNull() || filename.isEmpty()) {
        return;
    }

    /* Delete old res file if opened */
    if (!filename.isNull() && !filename.isEmpty() && res != nullptr) {
        ui->fileList->blockSignals(true);
        ui->fileList->clear();
        ui->fileList->blockSignals(false);
        delete res;
    }

    res = new ResFile(filename);
    if (!res->resFileOk()) {
        QMessageBox::critical(this, tr("Error parsing the file"),
          tr("The file doesn't appear to be a valid RES file, please retry."),
          QMessageBox::Ok);
    } else {
        const QStringList list = res->getFileList();
        ui->fileList->addItems(list);
        ui->fileList->setCurrentRow(0);

        /* Show the buttons for importing and exporting */
        ui->picExport->show();
        ui->picImport->show();
    }
}

void MainWindow::imageSelected(int row) {
    const QImage& img = res->getImage(row);
    ui->picInfo->setText(res->getImageInfo(row));
    ui->picLabel->setAlignment(Qt::AlignCenter);
    ui->picLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::importImage() {
    const int selected = ui->fileList->currentRow();

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Import an image..."), "",
                                                    tr("Images (*.png *.jpg *.bmp)"));
    if (filename.isNull() || filename.isEmpty())
        return;

    QImageReader reader(filename);
    if (!res->importSizeMatch(selected, reader.size())) {
        QMessageBox::critical(this, "Error importing the image",
                              "The dimension don't match, please select an image with the same dimension");
        return;
    }

    QImage img = reader.read();
    if (img.isNull()) {
        QMessageBox::critical(this, "Error importing the image",
                              reader.errorString());
        return;
    }

    res->importImage(selected, img);

    /* Load the new image */
    const QImage& newimg = res->getImage(selected);
    ui->picLabel->setPixmap(QPixmap::fromImage(newimg));
}

void MainWindow::exportImage() {
    QString path = QFileDialog::getSaveFileName(this, tr("Export image..."),
                                                "",
                                                tr("Images (*.png)"));
    if (path.isNull() || path.isEmpty())
        return;

    if (!path.endsWith(".png", Qt::CaseInsensitive))
        path += QString(".png");

    const int selected = ui->fileList->currentRow();
    const QImage& current = res->getImage(selected);
    bool ok = current.save(path, "png");
    if (!ok) {
        QMessageBox::critical(this, "Error saving file",
                              "Could not save the file");
    }
}

void MainWindow::saveResAs() {
    QString path = QFileDialog::getSaveFileName(this, tr("Save RES as..."),
                                                "",
                                                tr("RES file (*.res)"));
    if (path.isNull() || path.isEmpty())
        return;

    if (!path.endsWith(".res", Qt::CaseInsensitive))
        path += QString(".res");

    bool ok = res->save(path);
    if (!ok) {
        QMessageBox::critical(this, "Error saving file",
                              "Could not save the file");
    }
}

void MainWindow::showAbout() {
    const QString about("RES Editor\n"
                        "Version: " CURRENT_VERSION "\n"
                        "Creator: o-marshmallow\n"
                        "Big thanks to: KikiOnE (xda-developers)\n"
                        "\nFor more information, please check the official repo:\n"
                        "https://github.com/o-marshmallow/Powkiddy-RES-editor"
                        );
    QMessageBox::about(this, "About RES editor", about);
}
