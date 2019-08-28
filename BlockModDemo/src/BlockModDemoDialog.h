#ifndef BLOCKMODEDEMODIALOG_H
#define BLOCKMODEDEMODIALOG_H

#include <QDialog>

namespace Ui {
class BlockModDemoDialog;
}

class BlockModDemoDialog : public QDialog {
	Q_OBJECT

public:
	explicit BlockModDemoDialog(QWidget *parent = nullptr);
	~BlockModDemoDialog();

private:
	Ui::BlockModDemoDialog *ui;
};

#endif // BLOCKMODEDEMODIALOG_H
